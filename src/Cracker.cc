/*
 * Stegseek 0.3 - a steghide cracker
 * Copyright (C) 2020 Rick de Jager
 * 
 * Based on the work of Stefan Hetzl <shetzl@chello.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#include "error.h"
#include "Utils.h"
#include "common.h"
#include "Cracker.h"
#include "EmbData.h"
#include "MHashPP.h"
#include "Selector.h"
#include "Extractor.h"
#include "CvrStgFile.h"


Cracker::Cracker ()
{
	VerboseMessage vrs ;
	// read wordlist
	vrs.setMessage (_("[v] Using wordlist file \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	vrs.printMessage() ;
	wordlist = std::ifstream(Args.WordlistFn.getValue().c_str()) ;

	// get stegfile
	vrs.setMessage (_("[v] Using stegfile file \"%s\"."), Args.StgFn.getValue().c_str()) ;
	vrs.printMessage() ;

	// get output file
	if (Args.ExtFn.is_set()) {
		vrs.setMessage (_("[v] Using output file \"%s\"."), Args.ExtFn.getValue().c_str()) ;
		vrs.printMessage() ;
	}

	// Print threading info
	vrs.setMessage (_("[v] Running on %d threads."), Args.Threads.getValue()) ;
	vrs.printMessage() ;

	vrs.setMessage ("") ;
	vrs.printMessage() ;

	// Validate wordlist
	if (wordlist.fail()) {
		throw SteghideError (_("could not open the wordlist \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	}

	// General initialization
	// Load the Stegfile
	Globs.TheCvrStgFile = CvrStgFile::readFile (Args.StgFn.getValue().c_str()) ;

	// Look up these parameters once, to prevent function calls for each passphrase attempt
	bitsperembvalue = AUtils::log2_ceil<unsigned short> (Globs.TheCvrStgFile->getEmbValueModulus()) ;
	numSamples = Globs.TheCvrStgFile->getNumSamples() ;
	samplesPerVertex = Globs.TheCvrStgFile->getSamplesPerVertex() ;
	EmbValueModulus = Globs.TheCvrStgFile->getEmbValueModulus();
	embvaluesRequestedMagic = AUtils::div_roundup<unsigned long> (EmbData::NBitsMagic, bitsperembvalue) ;

	// init the attempts counter
	attempts = 0 ;
}

Cracker::~Cracker ()
{
	// Might have to add a destructor, as this class will be manhandling threads
	// And can be killed by an impatient user at any time
}
	
void Cracker::crack ()
{

	unsigned int threads = Args.Threads.getValue() ;

	// Load the wordlist into memory
	std::string line;
	// init queues
	WorkQueues.resize(Args.Threads.getValue()) ;
	{
		// First push the "empty" passphrase into a Queue, as it is probably not included in a wordlist
		line = "" ;
		WorkQueues[0].push(line) ;
		// Then push each line of the wordlist into the WorkQueue
		int i = 0;
		while (std::getline(wordlist, line)) {
			WorkQueues[i].push(line) ;
			wordlistLength += 1 ;
			i = (i + 1) % threads ;
		}
	}
	printf("[i] Read the entire wordlist (%u words), starting cracker\n", wordlistLength) ;
	// TODO; The above loop pushes the entire wordlist into RAM. The alternative would be to push items in batches.
	// This does mean that we'd lock/unlock a lot

	// Initialize threads
	std::vector<std::thread> ThreadPool ;
	stopped = false ;
	success = false ;
	bool metricsEnabled = Args.Verbosity.getValue() != QUIET ;

	// Add a thread to keep track of metrics
	if (metricsEnabled) {
		ThreadPool.push_back(std::thread([this] {metrics(); })) ;
	}

	// Add n worker threads
	for (unsigned int i = 0; i < threads; i++) {
		ThreadPool.push_back(std::thread([this, i] {consume(i); })) ;
	}

	// Join all worker threads
	for (unsigned int i = 0; i < threads; i++) {
		ThreadPool.back().join() ;
		ThreadPool.pop_back() ;
	}

	// Set stopped flag and terminate the metrics thread
	stopped = true ;
	if (metricsEnabled) {
		ThreadPool.back().join() ;
		ThreadPool.pop_back() ;
	}

	// Print a newline to clear metrics/debug messages
	printf("\n");

	// If we didn't find a passphrase, print a message
	if (!success) {
		printf("[!] Could not find a valid passphrase.\n") ;
	} else {
		// Re-extract the data with the confirmed passphrase.
		// This does mean we're throwing away one valid "embdata" object, but
		// that's not a bad trade-off to be able to use steghide's structure
		printf("[i] --> Found passphrase: \"%s\"\n\n", foundPassphrase.c_str()) ;
		extract(foundPassphrase) ;
	}
}

void Cracker::metrics ()
{
	while (!stopped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10)) ; 
		unsigned int a = attempts ;
		float percentage = 100.0f * ((float) a / (float) wordlistLength) ;
		printf("\r[ %u / %u ]  (%.2f%%)                 ",a , wordlistLength, percentage) ;
	}
}

// Take jobs and crack 'em
void Cracker::consume (int threadID)
{
	// TODO, we exit on empty queue as well, because we're loading the entire queue in one go
	// This won't work with batching
	while (!stopped && !WorkQueues[threadID].empty())
	{
		std::string passphraseCandidate;
		// Pop an item from the queue
		passphraseCandidate = WorkQueues[threadID].front() ;
		WorkQueues[threadID].pop() ;
		// Add to the perf metric
		attempts += 1 ;

		// Try extracting with this passphrase
		if (tryPassphrase(passphraseCandidate))
		{
			// Tell the other threads that they should stop
			stopped = true ;
			success = true ;
			foundPassphrase = passphraseCandidate;
		}
	}
}

bool Cracker::verifyMagic (std::string Passphrase)
{
	// Create a buf to keep track of rng collisions
	UWORD32 rngBuf[24*samplesPerVertex + 1] ;
	rngBuf[0] = 0 ;

	// Create a random number generator
	MHASH td = mhash_init(MHASH_MD5);
	mhash(td, Passphrase.data(), Passphrase.size() ) ;

	UWORD32 hash[4] ;
	mhash_deinit (td, hash) ;
	for (unsigned int i = 0 ; i < 4 ; i++) {
		rngBuf[0] ^= hash[i] ;
	}

	// Magic, "shm" in binary LE
	const int magics[24] = {1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0} ;
	if (samplesPerVertex * embvaluesRequestedMagic >= numSamples) {
		// TODO; In theory, we should error out if we hit this.
		// However, I've seen this error happen randomly on valid input files
		// so I'm leaving it as a "return false" for now
		// throw CorruptDataError (_("the stego data from standard input is too short to contain the embedded data.")) ;
		return false ;
	}
	EmbValue ev = 0 ;
	unsigned long sv_idx = 0;
	for (unsigned long i = 0 ; i < embvaluesRequestedMagic ; i++) {
		for (unsigned int j = 0 ; j < samplesPerVertex ; j++, sv_idx++) {
			// Calc next random number
			rngBuf[sv_idx+1] = (UWORD32) (rngBuf[sv_idx]*1367208549 + 1) ;
			// Check for RNG collisions. Should be fairly rare as numSample gets larger
			for (unsigned long i = 0; i <= sv_idx; i++) {
				if (rngBuf[i] == rngBuf[sv_idx+1]) {
					// In case we find an rng collision, just pretend the magic check was succesful.
					// The stricter steghide extractor will double check our work
					//
					// This case is rare
					return true;
				}
			}
			const UWORD32 valIdx = sv_idx + (((double) rngBuf[sv_idx+1] / (double) 4294967296.0) * ((double) (numSamples-sv_idx))) ;
			ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (valIdx)) % EmbValueModulus;
		}
		if (ev != magics[i]) {
			return false ;
		}
		ev = 0;
	}
	return true;
}

bool Cracker::tryPassphrase (std::string Passphrase)
{
	// We first check if the file magic makes sense
	// It's important we only do this once, for the beginning of the file
	if (! verifyMagic(Passphrase)) {
		return false ;
	}

	// In case the magic checks out, we have to do a proper extract attempt.
	// This does re-do the magic, but it's rare enough that that won't matter.

	EmbData embdata (EmbData::EXTRACT, Passphrase) ;
	Selector sel (numSamples, Passphrase) ;


	unsigned long sv_idx = 0 ;
	while (!embdata.finished()) {
		unsigned long embvaluesrequested = AUtils::div_roundup<unsigned long> (embdata.getNumBitsRequested(), bitsperembvalue) ;
		if (sv_idx + (samplesPerVertex * embvaluesrequested) >= numSamples) {
			// TODO; In theory, we should error out if we hit this.
			// However, I've seen this error happen randomly on valid input files
			// so I'm leaving it as a "return false" for now
			// throw CorruptDataError (_("the stego data from standard input is too short to contain the embedded data.")) ;
			return false ;
		}
		BitString bits (EmbValueModulus) ;
		for (unsigned long i = 0 ; i < embvaluesrequested ; i++) {
			EmbValue ev = 0 ;
			for (unsigned int j = 0 ; j < samplesPerVertex ; j++, sv_idx++) {
				ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (sel[sv_idx])) % EmbValueModulus ;
			}
			bits.appendNAry(ev) ;
		}

		// It's possible that we find wrong passphrase for which we decrypt a valid magic
		// Statistically, this only happens once every 256^3 times, but for a large wordlist, 
		// such as rockyou, this _can_ happen.
		try
		{
			embdata.addBits(bits) ;
		}
		// Catching other errors here is fine, they should be pretty rare.
		//	* decompression error
		//	* failed crc check
		catch(const SteghideError&)
		{
			return false ;
		}
	}
	// We've found a passphrase, for which both the file magic checks out
	// and the CRC is valid. :D
	return true ;
}

void Cracker::extract (std::string passphrase)
{
	Extractor ext (Args.StgFn.getValue(), passphrase) ;
	EmbData* emb = ext.extract() ;

	std::string origFn = emb->getFileName() ;
	std::string outFn = Utils::stripDir(Globs.TheCvrStgFile->getName()) + ".out" ;
	if (Args.ExtFn.is_set()) {
		outFn = Args.ExtFn.getValue() ;
	}

	if (origFn != "") {
		printf("[i] Original filename: \"%s\"\n", origFn.c_str()) ;
	}
	printf("[i] Extracting to \"%s\"\n", outFn.c_str()) ;
	BinaryIO io (outFn, BinaryIO::WRITE) ;
	std::vector<BYTE> data = emb->getData() ;
	for (std::vector<BYTE>::iterator i = data.begin() ; i != data.end() ; i++) {
		io.write8 (*i) ;
	}
	io.close() ;
	delete(emb) ;
}

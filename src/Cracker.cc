/*
 * Stegseek 0.1 - a steghide cracker
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
#include "common.h"
#include "Cracker.h"
#include "EmbData.h"
#include "Selector.h"
#include "Extractor.h"
#include "CvrStgFile.h"

using namespace std::chrono_literals ;


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

	// Print threading info
	vrs.setMessage (_("[v] Running on %d threads."), Args.Threads.getValue()) ;
	vrs.printMessage() ;

	vrs.setMessage ("") ;
	vrs.printMessage() ;
}

Cracker::~Cracker ()
{
	// Might have to add a destructor, as this class will be manhandling threads
	// And can be killed by an impatient user at any time
}
	
void Cracker::crack ()
{
	// Validate wordlist
	if (wordlist.fail()) {
		throw SteghideError (_("could not open the wordlist \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	}

	// Load the Stegfile
	Globs.TheCvrStgFile = CvrStgFile::readFile (Args.StgFn.getValue().c_str()) ;

	// Look up these parameters once, to prevent function calls for each passphrase attempt
	bitsperembvalue = AUtils::log2_ceil<unsigned short> (Globs.TheCvrStgFile->getEmbValueModulus()) ;
	numSamples = Globs.TheCvrStgFile->getNumSamples() ;
	samplesPerVertex = Globs.TheCvrStgFile->getSamplesPerVertex() ;
	EmbValueModulus = Globs.TheCvrStgFile->getEmbValueModulus();
	embvaluesRequestedMagic = AUtils::div_roundup<unsigned long> (EmbData::NBitsMagic, bitsperembvalue) ;

	// Load the wordlist into memory
	std::string line;
	{
		// Try to aquire a lock
		std::lock_guard<std::mutex> lk(QueueMutex) ;
		// First push the "empty" passphrase into the Queue, as it is probably not included in a wordlist
		line = "" ;
		WorkQueue.push(line) ;
		// Then push each line of the wordlist into the WorkQueue
		while (std::getline(wordlist, line)) {
			WorkQueue.push(line) ;
			wordlistLength += 1 ;
		}
	}
	printf("[i] Read the entire wordlist (%u words), starting cracker\n", wordlistLength) ;
	// TODO; The above loop pushes the entire wordlist into RAM. The alternative would be to push items in batches.
	// This does mean that we'd lock/unlock a lot

	// Initialize n threads
	std::vector<std::thread> ThreadPool ;
	stopped = false ;
	int threads = Args.Threads.getValue() ;

	for (int i = 0; i < threads; i++) {
		ThreadPool.push_back(std::thread([this] {consume(); })) ;
	}

	// Add a thread to keep track of metrics
	if (Args.Verbosity.getValue() != QUIET) {
		ThreadPool.push_back(std::thread([this] {metrics(); })) ;
		threads += 1 ;
	}

	// Join all threads
	for (int i = 0; i < threads; i++) {
		ThreadPool.back().join() ;
		ThreadPool.pop_back() ;
	}

	// Print a newline to clear metrics/debug messages
	printf("\n");

	// If we didn't find a passphrase, print a message
	if (!stopped) {
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
	while (!stopped && !WorkQueue.empty())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10)) ; 
		// This will be slightly incorrect, as we're not locking attempts
		float percentage = 100.0f * ((float) attempts / (float) wordlistLength) ;
		printf("\r[ %u / %u ]  (%.2f%%)                 ", attempts, wordlistLength, percentage) ;
	}
}

// Take jobs and crack 'em
void Cracker::consume ()
{
	while (!stopped && !WorkQueue.empty())
	{
		std::string passphraseCandidate;
		// Try to aquire a lock
		{
			std::lock_guard<std::mutex> lk(QueueMutex) ;

			// Another thread found the passphrase
			// TODO, we exit on empty queue as well, because we're loading the entire queue in one go
			// This won't work with batching
			if (stopped || WorkQueue.empty()) 
			{
				return ;
			}

			// Pop an item from the queue and unlock
			passphraseCandidate = WorkQueue.front() ;
			WorkQueue.pop() ;
			// Add to the perf metric
			attempts += 1 ;
		}

		// Try extracting with this passphrase
		if (tryPassphrase(passphraseCandidate))
		{
			// Tell the other threads that we have stopped
			stopped = true ;
			foundPassphrase = passphraseCandidate;
		}
	}
}

bool Cracker::verifyMagic (std::string Passphrase)
{
	//TODO; Still creates a selector object for each crack attempt
	Selector sel (numSamples, Passphrase) ;
	// Magic, "shm" in binary LE
	const int magics[] = {1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0} ;
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
			ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (sel[sv_idx])) % EmbValueModulus ;
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
	std::string outFn = Args.StgFn.getValue() + ".out" ;

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
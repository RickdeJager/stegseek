/*
 * [name] [version] - a steghide cracker
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



Cracker::Cracker ()
{
	VerboseMessage vrs ;
	// read wordlist
	vrs.setMessage (_("reading wordlist file \"%s\"..."), Args.WordlistFn.getValue().c_str()) ;
	vrs.printMessage() ;
	wordlist = std::ifstream(Args.WordlistFn.getValue().c_str()) ;

	// get stegfile
	vrs.setMessage (_("reading stegfile file \"%s\"..."), Args.StgFn.getValue().c_str()) ;
	vrs.printMessage() ;

	// Print threading info
	vrs.setMessage (_("Running on %d threads..."), Args.Threads.getValue()) ;
	vrs.printMessage() ;
}

Cracker::~Cracker ()
{
	// Might have to add a destructor, as this class will be manhandling threads
	// And can be killed by an impatient user at any time
}
	
void Cracker::crack ()
{
	// Load the Stegfile
	Globs.TheCvrStgFile = CvrStgFile::readFile (Args.StgFn.getValue().c_str()) ;

	// Look up these parameters once, to prevent function calls for each password attempt
	bitsperembvalue = AUtils::log2_ceil<unsigned short> (Globs.TheCvrStgFile->getEmbValueModulus()) ;
	numSamples = Globs.TheCvrStgFile->getNumSamples() ;
	samplesPerVertex = Globs.TheCvrStgFile->getSamplesPerVertex() ;
	EmbValueModulus = Globs.TheCvrStgFile->getEmbValueModulus();

	// Load the wordlist into memory
	std::string line;
	{
		// Try to aquire a lock
		std::lock_guard<std::mutex> lk(QueueMutex) ;
		// Push each line of the wordlist into the WorkQueue
		while (std::getline(wordlist, line)) {
			WorkQueue.push(line);
		}
	}
	printf("Read the entire wordlist, starting threads...\n") ;
	// TODO; The above loop pushes the entire wordlist into RAM. The alternative would be to push items in batches.
	// This does mean that we'd lock/unlock a lot

	// Initialize n threads
	std::vector<std::thread> ThreadPool ;
	stopped = false ;

	for (int i = 0; i < Args.Threads.getValue(); i++) {
		ThreadPool.push_back(std::thread([this] {consume(); })) ;
	}

	// Wait for either one of the threads to find the solution,
	// or the queue to become empty

	// Join all threads
	for (int i = 0; i < Args.Threads.getValue(); i++) {
		ThreadPool.back().join() ;
		ThreadPool.pop_back() ;
	}
}

// Take jobs and crack 'em
void Cracker::consume ()
{
	while (!stopped && !WorkQueue.empty())
	{
		std::string PasswordCandidate;
		// Try to aquire a lock
		{
			std::lock_guard<std::mutex> lk(QueueMutex) ;

			// Another thread found the password
			// TODO, we exit on empty queue as well, because we're loading the entire queue in one go
			// This won't work with batching
			if (stopped || WorkQueue.empty()) 
			{
				puts("Stopped...") ; 
				return ;
			}

			// Pop and item from the queue and unlock
			PasswordCandidate = WorkQueue.front() ;
			WorkQueue.pop() ;
		}

		// Try extracting with this password
		if (extract(PasswordCandidate))
		{
			printf("Found password: \"%s\"\n", PasswordCandidate.c_str()) ;
			// Tell the other threads that we have stopped
			stopped = true ;
		}
	}
}

bool Cracker::extract (std::string Passphrase)
{
	EmbData embdata (EmbData::EXTRACT, Passphrase) ;
	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Passphrase) ;

	bool magicOkay = false ;

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
		// We first check if the file magic makes sense
		// It's important we only do this once, for the beginning of the file
		if (! magicOkay) {
			if (bits.getValue(0, EmbData::NBitsMagic) != EmbData::Magic) {
				return false ;
			}
			magicOkay = true ;
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
		catch(SteghideError)
		{
			return false ;
		}
		catch(CorruptDataError)
		{
			return false ;
		}
		
	}
	// We've found a password, for which both the file magic checks out
	// and the CRC is valid. :D
	return true ;
}
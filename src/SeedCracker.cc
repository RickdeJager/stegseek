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
#include "SeedCracker.h"
#include "EmbData.h"
#include "MHashPP.h"
#include "Selector.h"
#include "Extractor.h"
#include "CvrStgFile.h"
#include "EncryptionMode.h"
#include "EncryptionAlgorithm.h"


SeedCracker::SeedCracker ()
{
	VerboseMessage vrs ;

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

SeedCracker::~SeedCracker ()
{
	// Might have to add a destructor, as this class will be manhandling threads
	// And can be killed by an impatient user at any time
}
	
void SeedCracker::crack ()
{
	// Initialize threads
	std::vector<std::thread> ThreadPool ;
	stopped = false ;
	success = false ;
	unsigned int threads = Args.Threads.getValue() ;
	bool metricsEnabled = Args.Verbosity.getValue() != QUIET ;

	// Add a thread to keep track of metrics
	if (metricsEnabled) {
		ThreadPool.push_back(std::thread([this] {metrics(); })) ;
	}

	// Add n worker threads
	unsigned int part = UWORD32_MAX / threads ;
	for (unsigned int i = 0; i < threads; i++) {
		ThreadPool.push_back(std::thread([this, i, part] {consume(i*part, (i+1)*part); })) ;
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
		printf("[!] Could not find a valid seed.\n") ;
	} else {
		// Re-extract the data with the confirmed passphrase.
		// This does mean we're throwing away one valid "embdata" object, but
		// that's not a bad trade-off to be able to use steghide's structure
		printf("[i] --> Found seed: \"%x\"\n\n", foundSeed) ;
	}
}

void SeedCracker::metrics ()
{
	while (!stopped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10)) ; 
		unsigned int a = attempts ;
		float percentage = 100.0f * ((float) a / (float) UWORD32_MAX) ;
		printf("\r[ %u / %u ]  (%.2f%%)                 ",a , UWORD32_MAX, percentage) ;
	}
}

// Take jobs and crack 'em
void SeedCracker::consume (unsigned int i, unsigned int stop)
{
	// TODO, we exit on empty queue as well, because we're loading the entire queue in one go
	// This won't work with batching
	while (!stopped && i < stop)
	{
		// Add to the perf metric
		attempts += 1 ;

		// Try extracting with this passphrase
		if (verifyMagic(i))
		{
			// Tell the other threads that they should stop
			stopped = true ;
			success = true ;
			foundSeed = i;
		}
		i++ ;
	}
}

bool SeedCracker::verifyMagic (UWORD32 seed)
{
	const int magics[24 + 1] = {
		// Magic, "shm" in binary LE
		1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0,
		// Code version, must be zero
		0,
	} ;

	// Magic + code version + enc mode + enc algo + plain size
	const unsigned int requestedValues = (24+1+3+5+32);

	// Create a buf to keep track of rng collisions
	UWORD32 rngBuf[requestedValues*samplesPerVertex + 1] ;
	rngBuf[0] = seed ;

	if (requestedValues*samplesPerVertex >= numSamples) {
		// TODO; In theory, we should error out if we hit this.
		// However, I've seen this error happen randomly on valid input files
		// so I'm leaving it as a "return false" for now
		//throw CorruptDataError (_("the stego data from standard input is too short to contain the embedded data.")) ;
		return false ;
	}
	EmbValue ev = 0 ;
	unsigned int encAlg = 0 ;
	unsigned int encMode = 0 ;
	unsigned int plainSize = 0 ;
	unsigned long sv_idx = 0;
	for (unsigned long i = 0 ; i < requestedValues ; i++) {
		for (unsigned int j = 0 ; j < samplesPerVertex ; j++, sv_idx++) {
			// Calc next random number
			rngBuf[sv_idx+1] = (UWORD32) (rngBuf[sv_idx]*1367208549 + 1) ;
			// Check for RNG collisions. Should be fairly rare as numSample gets larger
			for (unsigned long k = 0; k <= sv_idx; k++) {
				if (rngBuf[k] == rngBuf[sv_idx+1]) {
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
		if ( i < 25 && ev != magics[i]) {
			return false ;
		}
		// Get enc algo
		if (25 <= i && i < 30) {
			encAlg ^= ev << (i - 25) ;
			if (encAlg > 23) {
				return false ;
			}
		}
		// Get enc mode
		if (30 <= i && i < 33) {
			encMode ^= ev << (i - 30) ;
		}
		// get Plain size
		if (33 <= i && i < 65) {
			plainSize ^= ev << (i - 33) ;
			if (plainSize * samplesPerVertex > numSamples - requestedValues*samplesPerVertex ) {
				// This plain size wouldn't fit, so it must be wrong
				return false ;
			}
		}
		ev = 0;
	}
	printf("\nPlain size: %u bytes\n", plainSize / 8) ;
	printf("Enc Mode: %u (%s)\nEnc Algo: %u (%s)\n", encMode, EncryptionMode::translate(EncryptionMode::IRep(encMode)).c_str(),
													 encAlg, EncryptionAlgorithm::translate(EncryptionAlgorithm::IRep(encAlg)).c_str()) ;
	return true;
}
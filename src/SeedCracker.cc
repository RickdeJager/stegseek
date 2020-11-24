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

#include "SeedCracker.h"

#include "error.h"
#include "common.h"
#include "EmbData.h"
#include "Selector.h"
#include "Extractor.h"
#include "CvrStgFile.h"
#include "EncryptionMode.h"
#include "EncryptionAlgorithm.h"


void SeedCracker::crack ()
{
	// Initialize threads
	std::vector<std::thread> ThreadPool ;
	unsigned int threads = Args.Threads.getValue() ;
	bool metricsEnabled = Args.Verbosity.getValue() != QUIET ;

	// Add a thread to keep track of metrics
	if (metricsEnabled) {
		ThreadPool.push_back(std::thread([this] {metrics(UWORD32_MAX); })) ;
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

	// If we didn't find a passphrase, print a message
	if (!success) {
		printf("[!] Could not find a valid seed.\n") ;
	} else {
		// Output the found seed. At the moment this isn't particularly
		// useful to then end-user, it's nice for debugging
		printf("[i] --> Found seed: \"%x\"\n\n", foundResult.seed) ;
		finish() ;
	}
}

// Take jobs and crack 'em
void SeedCracker::consume (unsigned int i, unsigned int stop)
{
	while (!stopped && i < stop)
	{
		// Add to the perf metric
		attempts += 1 ;

		// Try extracting with this passphrase
		if (trySeed(i))
		{
			// Tell the other threads that they should stop
			stopped = true ;
			success = true ;
		}
		i++ ;
	}
}

bool SeedCracker::trySeed (UWORD32 seed)
{
	// First verify the magic
	if (!verifyMagic(seed)) {
		return false ;
	}
	// Next, try to interpret the remaining bits
	// enc mode + enc algo + plain size
	const unsigned int requestedValues = 3 + 5 + 32 ;

	// Create a "proper" selector object
	Selector sel (numSamples, seed) ;
	EmbValue ev = 0 ;
	unsigned int encAlgo = 0 ;
	unsigned int encMode = 0 ;
	unsigned int plainSize = 0 ;
	// We've already verified the magic (25 bits), so we can skip them
	unsigned long sv_idx = 25*samplesPerVertex ;
	for (unsigned long i = 0 ; i < requestedValues ; i++) {
		for (unsigned int j = 0 ; j < samplesPerVertex ; j++, sv_idx++) {
			ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (sel[sv_idx])) % EmbValueModulus;
		}
		// Get enc algo
		if (i < 5) {
			encAlgo ^= ev << i ;
			if (encAlgo > 23) {
				return false ;
			}
		}
		// Get enc mode
		if (5 <= i && i < 8) {
			encMode ^= ev << (i - 5) ;
		}
		// get Plain size
		if (8 <= i && i < 40) {
			plainSize ^= ev << (i - 8) ;
			if (plainSize * samplesPerVertex > numSamples - (25 + requestedValues) * samplesPerVertex ) {
				// This plain size wouldn't fit, so the seed must be wrong.
				return false ;
			}
		}
		ev = 0 ;
	}

	// Save the values on success
	foundResult = Result {
		seed,
		plainSize,
		encAlgo,
		encMode,
	} ;
	return true ;
}

void SeedCracker::finish()
{
	printf("\nPlain size: %u bytes (compressed)\n", foundResult.plainSize / 8) ;
	printf("Encryption Algorithm: %u (%s)\n", foundResult.encAlgo, 
			EncryptionAlgorithm::translate(EncryptionAlgorithm::IRep(foundResult.encAlgo)).c_str()) ;
	printf("Encryption Mode: %u (%s)\n", foundResult.encMode, 
			EncryptionMode::translate(EncryptionMode::IRep(foundResult.encMode)).c_str()) ;

	// Data is not encrypted :D
	// Let's dump it to a file
	if (foundResult.encAlgo == 0) {
		Extractor ext (Args.StgFn.getValue(), foundResult.seed) ;
		EmbData* emb = ext.extract() ;
		extract(emb) ;
		delete(emb) ;
	}
}
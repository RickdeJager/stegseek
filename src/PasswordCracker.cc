/*
 * Stegseek 0.5 - a steghide cracker
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

#include "PasswordCracker.h"

#include "error.h"
#include "Utils.h"
#include "common.h"
#include "EmbData.h"
#include "Selector.h"
#include "Extractor.h"
#include "CvrStgFile.h"


PasswordCracker::PasswordCracker ()
{
	VerboseMessage vrs ;
	vrs.setMessage (_("[v] Using wordlist file \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	vrs.printMessage() ;
}

void PasswordCracker::crack ()
{

	unsigned int threads = Args.Threads.getValue() ;

	// First stat the word list to get the file length
	struct stat wordlistStats ;
	if (stat(Args.WordlistFn.getValue().c_str(), &wordlistStats) != 0) {
		throw SteghideError("could not open the wordlist \"%s\".", Args.WordlistFn.getValue().c_str()) ;
	}

	// Initialize threads
	std::vector<std::thread> ThreadPool ;
	bool metricsEnabled = Args.Verbosity.getValue() != QUIET ;

	// Add a thread to keep track of metrics
	if (metricsEnabled) {
		ThreadPool.push_back(std::thread([this, wordlistStats] {metrics(wordlistStats.st_size, "bytes"); })) ;
	}

	// Add n worker threads
	unsigned long part = wordlistStats.st_size / threads ;
	for (unsigned int i = 0; i < threads; i++) {
		ThreadPool.push_back(std::thread([this, i, part, metricsEnabled] {
			consume(i*part, (i+1)*part, metricsEnabled); 
		})) ;
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

	if (!success) {
		// On failure, send the message directly to stderr so it will be printed in quiet mode as well
		fprintf(stderr, "[!] Could not find a valid passphrase.\n") ;
	} else {
		// Re-extract the data with the confirmed passphrase.
		// This does mean we're throwing away one valid "embdata" object, but
		// that's not a bad trade-off to be able to use steghide's structure
		fprintf(stderr, "[i] --> Found passphrase: \"%s\"\n", foundPassphrase.c_str()) ;
		extract(foundPassphrase) ;
	}
}

// Take jobs and crack 'em
void PasswordCracker::consume (unsigned long i, unsigned long stop, bool metricsEnabled)
{
	FILE * pWordList ;
	pWordList = fopen(Args.WordlistFn.getValue().c_str(), "r") ;
	// Skip to our section
	fseek(pWordList, i, SEEK_SET) ;

	char line[256] ;
	// We allow for some overlap here (i.e. thread n will repeat a few passwords from thread n+1)
	// The reason for this is that we don't know where the linebreaks are without reading the entire
	// file first, which would hold up (n-1) threads from actually doing useful things.
	//
	// Instead, we start n threads at different points in the wordlist, possible in the middle of a password.
	// this is fine, since the "previous" thread will always repeat the first password assigned to this thread.
	//
	while (!stopped && fgets(line, sizeof(line), pWordList) && ftell(pWordList) <= stop+sizeof(line)) {
		// Strip \n and \r, \rn
		line[strcspn(line, "\r\n")] = 0 ;

		// Try extracting with this passphrase
		if (tryPassphrase(line))
		{
			// Tell the other threads that they should stop
			stopped = true ;
			success = true ;
			foundPassphrase = std::string(line);
			break ;
		}

		// Incrementing an atomic int is quite costly, so don't do it if no one cares about its value
		if (metricsEnabled) {
			// Add 1, since the newline was stripped, but is present in the file size
			progress += std::strlen(line) + 1;
		}
	}

	fclose(pWordList) ;
}

bool PasswordCracker::tryPassphrase (char * Passphrase)
{
	// We first check if the file magic makes sense
	if (! verifyMagic(Passphrase)) {
		return false ;
	}

	std::string PassphraseStr(Passphrase) ;

	// In case the magic checks out, we have to do a proper extract attempt.
	// This does re-do the magic, but it's rare enough that that won't matter.

	EmbData embdata (EmbData::EXTRACT, PassphraseStr) ;
	Selector sel (numSamples, PassphraseStr) ;


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

void PasswordCracker::extract (std::string passphrase)
{
	Extractor ext (Args.StgFn.getValue(), passphrase) ;
	EmbData* emb = ext.extract() ;
	Cracker::extract(emb) ;
	delete(emb) ;
}

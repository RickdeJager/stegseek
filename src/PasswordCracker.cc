/*
 * Stegseek 0.4 - a steghide cracker
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


#include <sys/stat.h>
#include <string.h>

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
	// read wordlist
	vrs.setMessage (_("[v] Using wordlist file \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	vrs.printMessage() ;
	wordlist = std::ifstream(Args.WordlistFn.getValue().c_str()) ;

	// Validate wordlist
	if (wordlist.fail()) {
		throw SteghideError (_("could not open the wordlist \"%s\"."), Args.WordlistFn.getValue().c_str()) ;
	}

}

void PasswordCracker::crack ()
{

	unsigned int threads = Args.Threads.getValue() ;

	// First stat the word list to get the file length
	struct stat st ;
	stat(Args.WordlistFn.getValue().c_str(), &st) ;
	//TODO; This isn't accurate, since st_size is in bytes rather than words.
	wordlistLength = st.st_size ;

	// Initialize threads
	std::vector<std::thread> ThreadPool ;
	bool metricsEnabled = Args.Verbosity.getValue() != QUIET ;

	// Add a thread to keep track of metrics
	if (metricsEnabled) {
		ThreadPool.push_back(std::thread([this] {metrics(wordlistLength); })) ;
	}

	// Add n worker threads
	unsigned long part = wordlistLength / threads ;
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
		printf("[!] Could not find a valid passphrase.\n") ;
	} else {
		// Re-extract the data with the confirmed passphrase.
		// This does mean we're throwing away one valid "embdata" object, but
		// that's not a bad trade-off to be able to use steghide's structure
		printf("[i] --> Found passphrase: \"%s\"\n\n", foundPassphrase.c_str()) ;
		extract(foundPassphrase) ;
	}
}

// Take jobs and crack 'em
void PasswordCracker::consume (unsigned long i, unsigned long stop)
{
	FILE * pWordList ;
	pWordList = fopen(Args.WordlistFn.getValue().c_str(), "r") ;
	// Skip to our section
	fseek(pWordList, i, SEEK_SET) ;

	char line[256] ;
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
		attempts ++;
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

/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>
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
}

Cracker::~Cracker ()
{
	// Might have to add a destructor, as this class will be manhandling threads
	// And can be killed by an impatient user at any time
}
	
void Cracker::crack ()
{
	// Spawn n new threads, and tell each of them their ID.
	// Pass them both the steg file and wordlist file
	// Let each thread iterate over the wordlist, in steps of 'n'

	// TODO; move this to a header or config
	int threads = 1;

	// Load the Stegfile
	Globs.TheCvrStgFile = CvrStgFile::readFile (Args.StgFn.getValue().c_str()) ;

	for (int i = 0; i < threads; i++) {
		std::string password;
		while (std::getline(wordlist, password)) {
			if (extract(password)) {
				printf("Found password: \"%s\"\n", password.c_str()) ;
			}
		}
		printf("Reached the end of the wordlist.\n") ;
	}
}

bool Cracker::extract (std::string Passphrase)
{
	EmbData* embdata = new EmbData (EmbData::EXTRACT, Passphrase) ;
	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Passphrase) ;

	unsigned long sv_idx = 0 ;
	while (!embdata->finished()) {
		unsigned short bitsperembvalue = AUtils::log2_ceil<unsigned short> (Globs.TheCvrStgFile->getEmbValueModulus()) ;
		unsigned long embvaluesrequested = AUtils::div_roundup<unsigned long> (embdata->getNumBitsRequested(), bitsperembvalue) ;
		if (sv_idx + (Globs.TheCvrStgFile->getSamplesPerVertex() * embvaluesrequested) >= Globs.TheCvrStgFile->getNumSamples()) {
			if (Globs.TheCvrStgFile->is_std()) {
				// TODO; Error out here, as we'll never crack this file
				throw CorruptDataError (_("the stego data from standard input is too short to contain the embedded data.")) ;
				//return false ;
			}
			else {
				// TODO; Error out here, as we'll never crack this file
				throw CorruptDataError (_("the stego file \"%s\" is too short to contain the embedded data."), Globs.TheCvrStgFile->getName().c_str()) ;
				//return false ;
			}
		}
		BitString bits (Globs.TheCvrStgFile->getEmbValueModulus()) ;
		for (unsigned long i = 0 ; i < embvaluesrequested ; i++) {
			EmbValue ev = 0 ;
			for (unsigned int j = 0 ; j < Globs.TheCvrStgFile->getSamplesPerVertex() ; j++, sv_idx++) {
				ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue (sel[sv_idx])) % Globs.TheCvrStgFile->getEmbValueModulus() ;
			}
			bits.appendNAry(ev) ;
		}
		// TODO optimize this by getting rid of the try/catch structure.
		// We should be able to read the magic from the first few bits
		try
		{
			embdata->addBits (bits) ;
		}
		catch(SteghideError e)
		{
			return false ;
		}
		
	}


	if (embdata->checksumOK()) {
		return true ;
	}
	else {
		// TODO; Show a different error here, password was correct but data is corrupted
		//CriticalWarning w (_("crc32 checksum failed! extracted data is probably corrupted.")) ;
		return false ;
	}
}
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

#include "AUtils.h"
#include "BinaryIO.h"
#include "BitString.h"
#include "EmbData.h"
#include "error.h"
#include "MCryptPP.h"
#include "MHashPP.h"
#include "common.h"

EmbData::EmbData (MODE m, std::string pp, std::string fn)
	: Mode(m), Passphrase(pp), FileName(fn)
{
	if (m == EXTRACT) {
		NumBitsNeeded = NumBitsRequested = NBitsMagic ;
		Version = CodeVersion ;
		State = READ_MAGIC ;
		Reservoir = BitString() ;
	}
}

bool EmbData::finished ()
{
	myassert (Mode == EXTRACT) ;
	return (State == END) ;
}

unsigned long EmbData::getNumBitsRequested ()
{
	myassert (Mode == EXTRACT) ;
	return NumBitsRequested ;
}

void EmbData::addBits (BitString addbits)
{
	myassert (Mode == EXTRACT) ;

#ifdef DEBUG
	printDebug (1, "\nEmbData::addBits called with") ;
	printDebug (1, " addbits:") ;
	addbits.printDebug (1, 2) ;
	printDebug (1, " Reservoir:") ;
	Reservoir.printDebug (1, 2) ;
#endif

	Reservoir.append (addbits) ;
	BitString bits ;
	if (Reservoir.getLength() >= NumBitsNeeded) {
		bits = Reservoir.cutBits (0, NumBitsNeeded) ; // take exactly the first NumBitsNeeded bits from Reservoir | addbits
	}
	else { // Reservoir.getLength() < NumBitsNeeded
		myassert(false) ;
	}

#ifdef DEBUG
	printDebug (1, "bits is now:") ;
	bits.printDebug (1, 2) ;
#endif

	switch (State) {
		case READ_MAGIC:
		{
#ifdef DEBUG
			printDebug (1, "in the READ_MAGIC state") ;
#endif
			if (bits.getValue(0, NBitsMagic) == Magic) {
				NumBitsNeeded = 1 ;
				NumBitsRequested = AUtils::bminus<unsigned long> (NumBitsNeeded, Reservoir.getLength()) ;
				State = READ_VERSION ;
			}
			else {
				throw SteghideError (_("could not extract any data with that passphrase!")) ;
			}
			break ;
		}	

		case READ_VERSION:
		{
#ifdef DEBUG
			printDebug (1, "in the READ_VERSION state") ;
#endif
			if (bits[0] == true) {
				Version++ ;
				NumBitsNeeded = AUtils::bminus ((UWORD32) 1, Reservoir.getLength()) ;
			}
			else {
				if (Version > CodeVersion) {
					throw CorruptDataError (_("attempting to read an embedding of version %d but steghide %s only supports embeddings of version %d."), Version, VERSION, CodeVersion) ;
				}
				NumBitsNeeded = EncryptionAlgorithm::IRep_size + EncryptionMode::IRep_size ;
				NumBitsRequested = AUtils::bminus<unsigned long> (NumBitsNeeded, Reservoir.getLength()) ;
				State = READ_ENCINFO ;
			}
			break ;
		}

		case READ_ENCINFO: {
#ifdef DEBUG
			printDebug (1, "in the READ_ENCINFO state") ;
#endif

			unsigned int algo = (unsigned int) bits.getValue (0, EncryptionAlgorithm::IRep_size) ;
			if (EncryptionAlgorithm::isValidIntegerRep (algo)) {
				EncAlgo.setValue ((EncryptionAlgorithm::IRep) algo) ;
			}
			unsigned int mode = (unsigned int) bits.getValue (EncryptionAlgorithm::IRep_size, EncryptionMode::IRep_size) ;
			if (EncryptionMode::isValidIntegerRep (mode)) {
				EncMode.setValue ((EncryptionMode::IRep) mode) ;
			}

			NumBitsNeeded = NBitsNPlainBits ;
			NumBitsRequested = AUtils::bminus<unsigned long> (NumBitsNeeded, Reservoir.getLength()) ;
			State = READ_NPLAINBITS ;

#ifndef USE_LIBMCRYPT
			if (EncAlgo.getIntegerRep() != EncryptionAlgorithm::NONE) {
				throw SteghideError (_(
							"The embedded data is encrypted but steghide has been compiled without encryption\n"
							"support. To be able to read the embedded data, you have to install libmcrypt\n"
							"(http://mcrypt.sourceforge.net/) and recompile steghide.")) ;
			}
#endif
		break ; }

		case READ_NPLAINBITS: {
#ifdef DEBUG
			printDebug (1, "in the READ_NPLAINBITS state") ;
#endif
			
			NPlainBits = bits.getValue (0, NBitsNPlainBits) ;

#ifdef USE_LIBMCRYPT
			NumBitsNeeded = (UWORD32) MCryptPP::getEncryptedSize (EncAlgo, EncMode, NPlainBits) ;
#else
			NumBitsNeeded = NPlainBits ;
#endif
			NumBitsRequested = AUtils::bminus<unsigned long> (NumBitsNeeded, Reservoir.getLength()) ;

			State = READ_ENCRYPTED ;
		break ; }

		case READ_ENCRYPTED: {
#ifdef DEBUG
			printDebug (1, "in the READ_ENCRYPTED state") ;
#endif

			BitString plain ;
#ifdef USE_LIBMCRYPT
			if (EncAlgo.getIntegerRep() == EncryptionAlgorithm::NONE) {
				plain = bits ;
			}
			else {
				MCryptPP crypto (EncAlgo, EncMode) ;
				plain = crypto.decrypt (bits, Passphrase) ;
			}
#else
			plain = bits ;
#endif

			plain.truncate (0, NPlainBits) ;	// cut off random padding used to achieve full number of encryption blocks

			unsigned long pos = 0 ;

			// read Compression (and uncompress)
			Compression = ((plain[pos++]) ? 9 : 0) ; // to make compression contain a value that makes sense
#ifdef DEBUG
			printDebug (2, " compression: %d\n", plain[pos - 1]) ;
#endif
			if (Compression > 0) {
				UWORD32 NUncompressedBits = plain.getValue (pos, NBitsNUncompressedBits) ;
#ifdef DEBUG
				printDebug (2, " nuncobits: %lu\n", NUncompressedBits) ;
#endif
				pos += NBitsNUncompressedBits ;

				plain.truncate (pos, plain.getLength()) ;
				pos = 0 ;
				plain.uncompress (NUncompressedBits) ;
			}

			// read Checksum
			Checksum = plain[pos++] ;
#ifdef DEBUG
			printDebug (2, " checksum: %d\n", plain[pos - 1]) ;
#endif
			if (Checksum) {
				CRC32 = plain.getValue (pos, NBitsCrc32) ;
#ifdef DEBUG
				printDebug (2, " crc32: 0x%x\n", CRC32) ;
#endif
				pos += NBitsCrc32 ;
			}

			// read filename
			char curchar = '\0' ;
			FileName = "" ;
			do {
				curchar = (char) plain.getValue (pos, 8) ;
				if (curchar != '\0') {
					FileName += curchar ;
				}
				pos += 8 ;
			} while (curchar != '\0') ;

			// extract data
			if ((plain.getLength() - pos) % 8 != 0) {
				throw CorruptDataError (_("the embedded data has an invalid length.")) ;
			}
			const unsigned long extdatalen = (plain.getLength() - pos) / 8 ;
			Data.resize (extdatalen) ;
			for (unsigned int i = 0 ; i < extdatalen ; i++) {
				Data[i] = ((BYTE) plain.getValue (pos, 8)) ;
				pos += 8 ;
			}

			NumBitsNeeded = 0 ;
			NumBitsRequested = 0 ;
			State = END ;
		break ; }

		case END:
		default: {
			myassert (0) ;
		break ; }
	}
}

bool EmbData::checksumOK (void) const
{
	// test if checksum is ok
	bool ok = true ;
	if (Checksum) {
		MHashPP hash (MHASH_CRC32) ;
		for (std::vector<BYTE>::const_iterator i = Data.begin() ; i != Data.end() ; i++) {
			hash << *i ;
		}
		hash << MHashPP::endhash ;
		unsigned long calccrc32 = hash.getHashBits().getValue(0, NBitsCrc32) ;

		if (calccrc32 == CRC32) {
			ok = true ;
		}
		else {
			ok = false ;
		}
	}
	return ok ;
}

void EmbData::setEncAlgo (EncryptionAlgorithm a)
{
	EncAlgo = a ;
}
	
EncryptionAlgorithm EmbData::getEncAlgo () const
{
	return EncAlgo ;
}
	
void EmbData::setEncMode (EncryptionMode m) 
{
	EncMode = m ;
}
	
EncryptionMode EmbData::getEncMode () const
{
	return EncMode ;
}
	
void EmbData::setCompression (int c)
{
	Compression = c ;
}
	
int EmbData::getCompression (void) const
{
	return Compression ;
}
	
void EmbData::setChecksum (bool c)
{
	Checksum = c ;
}
	
bool EmbData::getChecksum (void) const
{
	return Checksum ;
}

BitString EmbData::getBitString ()
{
	myassert (Mode == EMBED) ;

	// assembling data that can be compressed
	BitString compr ;

	compr.append (Checksum) ;
	if (Checksum) {
		MHashPP hash (MHASH_CRC32) ;
		for (std::vector<BYTE>::iterator i = Data.begin() ; i != Data.end() ; i++) {
			hash << *i ;
		}
		hash << MHashPP::endhash ;
		compr.append (hash.getHashBits()) ;
	}
	
	compr.append (stripDir (FileName)) ;
	compr.append ((BYTE) 0, 8) ; // end of fileame
	
	compr.append (Data) ;

	// assembling data that can be encrypted
	BitString plain ;
	plain.append ((Compression > 0) ? true : false) ;
	if (Compression > 0) {
		plain.append (compr.getLength(), NBitsNUncompressedBits) ;
		compr.compress (Compression) ;
	}
	plain.append (compr) ;

	// put it all together
	BitString main ;
	main.append(Magic, NBitsMagic) ;
	for (unsigned short i = 0 ; i < CodeVersion ; i++) {
		main.append(true) ;
	}
	main.append(false) ; // end of version
	main.append((UWORD16) EncAlgo.getIntegerRep(), EncryptionAlgorithm::IRep_size) ;
	main.append((UWORD16) EncMode.getIntegerRep(), EncryptionMode::IRep_size) ;
	main.append(plain.getLength(), NBitsNPlainBits) ;

#ifdef USE_LIBMCRYPT
	if (EncAlgo.getIntegerRep() != EncryptionAlgorithm::NONE) {
		MCryptPP crypto (EncAlgo, EncMode) ;
		plain = crypto.encrypt (plain, Passphrase) ;
	}
#else
	myassert (EncAlgo.getIntegerRep() == EncryptionAlgorithm::NONE) ;
#endif

	main.append (plain) ;

	return main ;
}

std::string EmbData::stripDir (std::string s)
{
	unsigned int start = 0 ;
	if ((start = s.find_last_of ("/\\")) == std::string::npos) {
		start = 0 ;
	}
	else {
		start += 1 ;
	}
	return s.substr (start, std::string::npos) ;
}

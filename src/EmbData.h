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

#ifndef SH_EMBDATA_H
#define SH_EMBDATA_H

#include <string>
#include <vector>

#include "common.h"

#include "BitString.h"
#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"

class EmbData {
	public:
	enum MODE { EMBED, EXTRACT } ;
	enum STATE { READ_MAGIC, READ_VERSION, READ_ENCINFO, READ_NPLAINBITS, READ_ENCRYPTED, END } ;

	/**
	 * construct a new EmbData object
	 * \param m the mode (EMBED or EXTRACT)
	 * \param pp the passphrase
	 * \param fn the filename (only need for mode EMBED)
	 **/
	EmbData (MODE m, std::string pp, std::string fn = "") ;

	BitString getBitString (void) ;

	bool finished (void) ;

	/**
	 * get the minimum length of the BitString that is to be passed to addBits
	 **/
	unsigned long getNumBitsRequested (void) ;

	void addBits (BitString addbits) ;

	void setEncAlgo (EncryptionAlgorithm a) ;
	EncryptionAlgorithm getEncAlgo (void) const ;
	
	void setEncMode (EncryptionMode m) ;
	EncryptionMode getEncMode (void) const ;

	void setCompression (int c) ;
	int getCompression (void) const ;

	void setChecksum (bool c) ;
	bool getChecksum (void) const ;

	/**
	 * check if crc32 checksum is ok (needs filled Data and CRC32 fields)
	 * \return true iff checksum is ok
	 **/
	bool checksumOK (void) const ;

	void setData (const std::vector<BYTE> data)
		{ Data = data ; } ;

	std::vector<BYTE> getData (void) const
		{ return Data ; } ;

	std::string getFileName (void) const
		{ return FileName ; } ;

	/// the minimum size of the part of the generatred BitString that is not the data
	static const unsigned int MinStegoHeaderSize = 50 ;

	protected:
	std::string stripDir (std::string s) ;

	private:
	/// number of bits used to code the number of plain bits
	static const unsigned int NBitsNPlainBits = 32 ;
	/// number of bits used to code the number of uncompressed bits
	static const unsigned int NBitsNUncompressedBits = 32 ;
	/// size of a crc32 checksum in bits
	static const unsigned int NBitsCrc32 = 32 ;
	/// version of this steghide embedding (stego compatibility of EmbData)
	static const unsigned short CodeVersion = 0 ;
	/// steghide magic to recognize embedded data (the string "shm")
	static const UWORD32 Magic = 0x73688DUL ;
	/// size (in bits of Magic)
	static const unsigned int NBitsMagic = 24 ;

	MODE Mode ;
	STATE State ;
	
	unsigned long NPlainBits ;

	/// the number of bits that the caller must at least supply to addBits
	unsigned long NumBitsRequested ;
	/// exactly the number of bits that the next step will consume from Reservoir and addBits together
	unsigned long NumBitsNeeded ;

	BitString Reservoir ;

	std::string Passphrase ;

	/// version read from input bitstring
	unsigned short Version ;

	EncryptionAlgorithm EncAlgo ;
	EncryptionMode EncMode ;
	/// compression level: 0(none),1(best speed),...,9(best compression)
	int Compression ;
	/// will a checksum be embedded ?
	bool Checksum ;
	/// the checksum
	unsigned long CRC32 ;
	std::string FileName ;
	/// contains the actual message to be embedded
	std::vector<BYTE> Data ;
} ;

#endif // ndef SH_EMBDATA_H

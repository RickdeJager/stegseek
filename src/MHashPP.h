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

#ifndef SH_MHASHPP_H
#define SH_MHASHPP_H

#include <mhash.h>

#include "common.h"

class BitString ;

class MHashPP {
	public:
	enum Command { endhash } ;

	MHashPP (void) ;
	MHashPP (hashid a) ;

	void init (hashid a) ;

	const std::vector<BYTE>& end (void) ;

	/**
	 * feed the std::string v to the hashing algorithm
	 * \param v the std::string to be feeded to the hashing algorithm (without '\0' at the end)
	 **/
	MHashPP& operator<< (std::string v) ;

	/**
	 * feed the BitString v to the hashing algorithm
	 * \param v the BitString to be feeded to the hashing algorithm (v.getLength() % 8 == 0 must hold)
	 **/
	MHashPP& operator<< (BitString v) ;

	/**
	 * feed the byte v to the hashing algorithm
	 * \param v the byte to be feeded to the hashing algorithm
	 **/
	MHashPP& operator<< (BYTE v) ;

	/**
	 * interpret the command c
	 * \param c a command (member of MHashPP::Command)
	 **/
	MHashPP& operator<< (Command c) ;

	/**
	 * get the hash bits
	 * \return the hash value of the data that has been passed via <<
	 **/
	BitString getHashBits (void) ;

	const std::vector<BYTE>& getHashBytes (void) ;

	/**
	 * get the hash size
	 * \return the size of the value returned by getHashBits in bytes
	 **/
	unsigned int getHashSize (void) ;

	private:
	/// true iff HashD contains a legal hash descriptor and data can be passed via <<
	bool hashing ;
	MHASH HashD ;

	/// true iff HashBytes contains a valid hash value
	bool HashBytesValid ;
	std::vector<BYTE> HashBytes ;

	std::string getAlgorithmName (void) ;
	static std::string getAlgorithmName (hashid id) ;
} ;

#endif // ndef SH_MHASHPP_H

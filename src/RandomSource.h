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

#ifndef SH_RANDOMSOURCE_H
#define SH_RANDOMSOURCE_H

#include <cstdio>
#include <vector>

class BitString ;

/**
 * \class RandomSource
 * \brief objects of this class are used as a source of random (non reproduceable) data
 **/
class RandomSource {
	public:
	RandomSource (void) ;
	~RandomSource (void) ;

	/**
	 * get a random byte
	 * \return a random byte
	 **/
	BYTE getByte (void) ;

	/**
	 * get n random bytes
	 * \param n the number of requested random bytes
	 * \return n random bytes
	 **/
	std::vector<BYTE> getBytes (unsigned int n) ;

	/**
	 * get n random bits
	 * \param n the number of requested random bits
	 * \return a BitString containing n random bits
	 **/
	BitString getBits (unsigned int n) ;

	/**
	 * get a boolean value
	 * \return true of false with equal probability
	 **/
	bool getBool (void) ;

	/**
	 * get a random value
	 * \param n the range of the random value to be returned
	 * \return a random number in {0,...,n-1}
	 **/
	unsigned long getValue (unsigned long n) ;

	private:
	unsigned int RandomBytePos ;
	BYTE RandomByte ;
	/**
	 * determines the random input - is either opened file pointer to
	 * /dev/urandom or NULL (the rand() function is then used as random source)
	 **/
	FILE *RandomInput ;
} ;

#endif // ndef SH_RANDOMSOURCE_H

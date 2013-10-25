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

#include <cstdlib>
#include <ctime>

#include "BitString.h"
#include "common.h"
#include "error.h"
#include "msg.h"
#include "RandomSource.h"

// the global RandomSource object
RandomSource RndSrc ;

RandomSource::RandomSource ()
{
	RandomBytePos = 8 ;
#ifdef HAVE_DEV_URANDOM
	if ((RandomInput = fopen ("/dev/urandom", "r")) == NULL) {
		Warning w (_("could not open /dev/urandom, using standard library random numbers instead.")) ;
		w.printMessage() ;
		RandomInput = NULL ;
		srand ((unsigned int) time (NULL)) ;
	}
#else
	RandomInput = NULL ;
	srand ((unsigned int) time (NULL)) ;
#endif
}

RandomSource::~RandomSource()
{
	if (RandomInput != NULL) {
		if (fclose (RandomInput) != 0) {
			throw SteghideError (_("could not close random input file.")) ;
		}
	}
}

BYTE RandomSource::getByte ()
{
	BYTE retval = 0 ;
#ifndef NORANDOM
	if (RandomInput != NULL) {
		retval = getc (RandomInput) ;
	}
	else {
		retval = (BYTE) (256.0 * (rand() / (RAND_MAX + 1.0))) ;
	}
#endif
	return retval ;
}

std::vector<BYTE> RandomSource::getBytes (unsigned int n)
{
	std::vector<BYTE> retval ;
	for (unsigned int i = 0 ; i < n ; i++) {
		retval.push_back (getByte()) ;
	}
	return retval ;
}

BitString RandomSource::getBits (unsigned int n)
{
	BitString retval ;
	BYTE rndbyte = 0 ;
	unsigned int bitsused = 8 ;
	while (n > 0) {
		if (bitsused == 8) {
			rndbyte = getByte() ;
			bitsused = 0 ;
		}
		retval.append ((BIT) (rndbyte & 1)) ;
		rndbyte = rndbyte >> 1 ;
		bitsused++ ;
		n-- ;
	}
	return retval ;
}

bool RandomSource::getBool()
{
	if (RandomBytePos == 8) {
		RandomByte = getByte() ;
		RandomBytePos = 0 ;
	}
	bool retval = (RandomByte & (1 << RandomBytePos)) ;
	RandomBytePos++ ;
	return retval ;
}

unsigned long RandomSource::getValue (unsigned long n)
{
	const unsigned long ceilvalue = 0x1000000UL ;
	myassert (n < ceilvalue) ;
	unsigned long value = ((unsigned long) (getByte() << 16)) |
						  ((unsigned long) (getByte() << 8)) |
						  ((unsigned long) getByte()) ;

	return (unsigned long) ((((double) value) / ((double) ceilvalue)) * ((double) n)) ;
}

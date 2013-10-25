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

#ifndef SH_PSEUDORANDOMSOURCE_H
#define SH_PSEUDORANDOMSOURCE_H

#include "common.h"
/**
 * \class PseudoRandomSource
 * \brief this class serves as a source of reproducible (pseudo-)random numbers
 *
 * To generate the random numbers, the linear congruetial method is used.
 * 2^32 is used as modulus. The overflow in the implementation is intended (and
 * controlled, as UWORD32 is used as datatype which always is 32 bits wide).
 **/
class PseudoRandomSource {
	public:
	/**
	 * construct a PseudoRandomSource object
	 * \param s the seed
	 **/
	PseudoRandomSource (UWORD32 s) : Value(s) {} ;

	/**
	 * get a pseudo-random value from {0,...,n-1}
	 * \param n the range of the random value to be returned
	 * \return a number >= 0 and <= n - 1
	 *
	 * After calling getValue, the next getValue call will use the next state
	 * of the random number generator (analogous to the C rand() function)
	 **/
	UWORD32 getValue (UWORD32 n) ;

	private:
	static const UWORD32 A = 1367208549 ;
	static const UWORD32 C = 1 ;
	UWORD32 Value ;
} ;

#endif // ndef SH_PSEUDORANDOMSOURCE_H

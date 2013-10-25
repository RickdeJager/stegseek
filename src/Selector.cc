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

#include "BitString.h"
#include "MHashPP.h"
#include "PseudoRandomSource.h"
#include "Selector.h"

Selector::Selector (UWORD32 m, std::string pp)
	: Maximum(m), NumInArray(0)
{
	MHashPP hash (MHASH_MD5) ;
	hash << pp << MHashPP::endhash ;
	BitString h = hash.getHashBits() ;

	myassert (h.getLength() == 128) ;
	UWORD32 seed = 0 ;
	for (unsigned short i = 0 ; i < 4 ; i++) {
		seed ^= h.getValue (i * 32, 32) ;
	}

	PRandom = new PseudoRandomSource (seed) ;
}

Selector::Selector (UWORD32 m)
{
	Maximum = m ;
	X.resize(m) ;
	for (UWORD32 i = 0 ; i < m ; i++) {
		X[i] = i ;
	}
	NumInArray = m ;
	PRandom = NULL ;
}

Selector::Selector (const std::vector<UWORD32>& retvals)
{
	X = retvals ;
	Maximum = retvals.size() ;
	NumInArray = retvals.size() ;
	PRandom = NULL ;
}

Selector::~Selector ()
{
	if (PRandom) {
		delete PRandom ;
	}
}

UWORD32 Selector::operator[] (UWORD32 i)
{
	myassert (i < Maximum) ;
	calculate(i + 1) ;
	return X[i] ;
}

void Selector::calculate (UWORD32 m)
{
	UWORD32 j = NumInArray ;

	if (m > NumInArray) {
		NumInArray = m ;
		X.resize (NumInArray) ;
		Y.resize (NumInArray) ;
	}

	for ( ; j < m ; j++) {
		// k is a number from {j,...,Maximum-1}
		UWORD32 k = j + PRandom->getValue(Maximum - j) ;

		UWORD32 i = 0 ;
		if (idxX (k, j, &i)) { // there exists i < j with X[i] = k
			// use Y[i] instead of k
			setX (j, Y[i]) ;

			if (X[j] > j) { // some future k could be X[j]
				Y[j] = j ;
			}
			if (X[i] > j) { // some future k could be X[i]
				Y[i] = j ;
				UWORD32 l = 0 ;
				if (idxX (Y[i], j, &l)) {
					Y[i] = Y[l] ;
				}
			}
		}
		else {
			setX (j, k) ;
			Y[j] = j ;
		}

		if (X[j] > j) {
			UWORD32 i = 0 ;
			if (idxX (Y[j], j, &i)) {
				Y[j] = Y[i] ;
			}
		}
	}
}

bool Selector::idxX (UWORD32 v, UWORD32 m, UWORD32* p) const
{
	bool retval = false ;
	sgi::hash_map<UWORD32,UWORD32>::const_iterator it = Xreversed.find (v) ;
	if ((it != Xreversed.end()) && (it->second < m)) {
		*p = it->second ;
		retval = true ;
	}
	return retval ;
}

void Selector::setX (UWORD32 i, UWORD32 v)
{
	X[i] = v ;
	Xreversed[v] = i ;
}

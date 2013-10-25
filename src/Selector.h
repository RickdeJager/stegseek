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

#ifndef SH_SELECTOR_H
#define SH_SELECTOR_H

#include <string>
#include <vector>
#include "wrapper_hash_map.h"

#include "common.h"

class PseudoRandomSource ;

/**
 * \class Selector
 * \brief a random permutation of a random combination
 **/
class Selector {
	public:
	/**
	 * construct a Selector object
	 * \param m the range - operator[] will return numbers in {0,...,m-1}
	 * \param pp a passphrase used to generate the seed
	 **/
	Selector (UWORD32 m, std::string pp) ;

	/**
	 * construct a Selector object that resembles the identity permutation with range m
	 * \param m the range - (*this)[i] will be i for i in {0,...,m-1}
	 **/
	Selector (UWORD32 m) ;

	/**
	 * construct a Selector object that will return predefined values
	 * \param retvals the values that operator[] shall return
	 *
	 * The Maximum is set to retvals.size().
	 **/
	Selector (const std::vector<UWORD32>& retvals) ;

	~Selector (void) ;

	/**
	 * get the value at the i-th position
	 * \param i an index
	 **/
	UWORD32 operator[] (UWORD32 i) ;

	UWORD32 getRange (void) const
		{ return Maximum ; } ;

	private:
	std::vector<UWORD32> X ;
	std::vector<UWORD32> Y ;
	sgi::hash_map<UWORD32,UWORD32> Xreversed ;
	/// operator[] will return a value in {0,...,Maximum-1}
	UWORD32 Maximum ;
	/// the number of calculated positions in the arrays
	UWORD32 NumInArray ;
	PseudoRandomSource* PRandom ;

	/**
	 * fill X, Y and Xreversed until but not including index m
	 **/
	void calculate (UWORD32 m) ;

	/**
	 * search an index i such that 0 <= i < m and X[i] == v
	 * \param v the value to be searched for in the X-array
	 * \param m the upper bound for the index
	 * \param p a pointer to a variable where the found index can be stored
	 * \return true iff such an index is found (*p is set to i in this case
	 **/
	bool idxX (UWORD32 v, UWORD32 m, UWORD32* p) const ;

	/**
	 * set X[i] to v while maintaining Xreversed
	 **/
	void setX (UWORD32 i, UWORD32 v) ;
} ;

#endif // ndef SH_SELECTOR_H

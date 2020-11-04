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

#ifndef SH_SAMPLEVALUETEST_H
#define SH_SAMPLEVALUETEST_H

#include <string>

#include "UnitTest.h"

class SampleValue ;

class SampleValueTest : public UnitTest {
	public:
	SampleValueTest (std::string n, TestSuite* s) : UnitTest(n, s) {} ;

	protected:
	/**
	 * generic distance test
	 * \param s1 a sample value
	 * \param s2 another sample value
	 * \param dist this should be the distance between s1 and s2
	 * \return true iff distance is commutative and return dist
	 **/
	bool genericTestDistance (SampleValue* s1, SampleValue* s2, UWORD32 dist) ;

	/**
	 * generic neighbourhood test
	 * \param s1 a sample value
	 * \param s2 another sample value
	 * \param res true iff they are neighbours
	 * \return true iff calling isNeighbour (with reflexivity) equals res
	 **/
	bool genericTestIsNeighbour (SampleValue* s1, SampleValue* s2, bool res) ;

	/**
	 * generic evalue test
	 * \param s a sample value
	 * \param ev an embedded value
	 * \return true iff s->getEmbeddedValue() == ev
	 **/
	bool genericTestEValue (SampleValue* s, EmbValue ev) ;
} ;

#endif // ndef SH_SAMPLEVALUETEST_H

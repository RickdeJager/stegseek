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

#ifndef SH_DUMMYSAMPLEVALUE_H
#define SH_DUMMYSAMPLEVALUE_H

#include "SampleValue.h"
#include "common.h"

class DummySampleValue : public SampleValue {
	public:
	DummySampleValue (UWORD16 v)
		: SampleValue(), Value(v)
		{ EValue = (v % 2) ; Key = v ; } ;

	UWORD16 getValue (void) const
		{ return Value ; } ;

	/**
	 * calculate the distance as | Value - s->Value |
	 **/
	UWORD32 calcDistance (const SampleValue* s) const ;

	/**
	 * return from the contents of the SampleValueAdjacencyMatrix in the DummyFile
	 **/
	bool isNeighbour (const SampleValue* s) const ;

	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;

	std::string getName (void) const ;

	private:
	UWORD16 Value ;
} ;

#endif // ndef SH_DUMMYSAMPLEVALUE_H

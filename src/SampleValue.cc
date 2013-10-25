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

#include "common.h"
#include "SampleValue.h"
#include "CvrStgFile.h"

SampleValue::SampleValue ()
	: Label(0)
{
	NumEdges = new UWORD32[Globs.TheCvrStgFile->getEmbValueModulus()] ;
}

SampleValue::~SampleValue ()
{
	delete[] NumEdges ;
}

void SampleValue::incNumEdges (EmbValue t)
{
	NumEdges[t]++ ;
}

void SampleValue::decNumEdges (EmbValue t)
{
	myassert (NumEdges[t] > 0) ;
	NumEdges[t]-- ;
}

bool SampleValue::isNeighbour (const SampleValue *s) const
{
#ifdef DEBUG
	printDebug (5, "SampleValue::isNeighbour: Distance: %lu, Radius: %lu\n", calcDistance(s), Globs.TheCvrStgFile->getRadius()) ;
#endif
	return (calcDistance (s) <= Globs.TheCvrStgFile->getRadius()) ;
}

void SampleValue::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;
	std::cerr << space << "SampleValue:" << std::endl ;
	std::cerr << space << " Label: " << getLabel() << std::endl ;
	std::cerr << space << " Name: " << getName() << std::endl ;
	std::cerr << space << " Key(hex): " << std::hex << getKey() << std::dec << std::endl ;
	std::cerr << space << " EValue: " << (unsigned int) getEmbeddedValue() << std::endl ;
}

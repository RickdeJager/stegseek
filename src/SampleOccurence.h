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

#ifndef SH_SAMPLEOCCURENCE_H
#define SH_SAMPLEOCCURENCE_H

#include "Vertex.h"

class SampleOccurence {
	public:
	SampleOccurence (Vertex *v, unsigned short i)
		: TheVertex(v), Index(i) {} ;

	Vertex *getVertex (void) const
		{ return TheVertex ; } ;

	void setVertex (Vertex* v)
		{ TheVertex = v ; } ;

	unsigned short getIndex (void) const
		{ return Index ; } ;

	void setIndex (unsigned short i)
		{ Index = i ; } ;

	bool operator== (const SampleOccurence& soc) const
		{ return ((TheVertex->getLabel() == soc.TheVertex->getLabel()) && (Index == soc.Index)) ; } ;

	private:
	Vertex *TheVertex ;
	unsigned short Index ;
} ;

#endif // ndef SH_SAMPLEOCCURENCE_H

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

#ifndef SH_EDGE_H
#define SH_EDGE_H

#include "SampleValue.h"
#include "Vertex.h"
#include "common.h"

class Edge {
	public:
	/**
	 * default constructor - does not create a useful object
	 **/
	Edge (void)
		: Vertex1(NULL), Index1(0), Vertex2(NULL), Index2(0), Weight(UWORD32_MAX) {} ;

	/**
	 * constructs an edge object
	 **/
	Edge (Vertex *v1, unsigned short idx1, Vertex *v2, unsigned short idx2) ;

	/**
	 * copy constructor
	 **/
	Edge (const Edge& e) ;

	Vertex *getVertex1 (void) const
		{ return Vertex1 ; } ;

	void setVertex1 (Vertex* v)
		{ Vertex1 = v ; } ;

	Vertex *getVertex2 (void) const
		{ return Vertex2 ; } ;

	unsigned short getIndex1 (void) const
		{ return Index1 ; } ;

	void setIndex1 (unsigned short i)
		{ Index1 = i ; } ;

	unsigned short getIndex2 (void) const
		{ return Index2 ; } ;

	UWORD32 getWeight (void) ;

	void set (Vertex* v1, unsigned short idx1, Vertex* v2, unsigned short idx2) ;

	void set1 (Vertex* v1, unsigned short idx1) ;

	void set2 (Vertex* v2, unsigned short idx2) ;

	bool operator== (const Edge& e) const ;

	bool operator!= (const Edge& e) const ;

	/**
	 * swap vertices 1 and 2 in this edge (weight is not altered)
	 **/
	void swap (void) ;

	/**
	 * \return true iff this edge contains the vertex v
	 **/
	bool contains (const Vertex *v) const ;

	/**
	 * get the vertex on this edge that is not equal to v
	 **/
	Vertex *getOtherVertex (const Vertex *v) const ;

	/**
	 * get the position of the sample that should be changed to embed the bit represented by the vertex v
	 **/
	SamplePos getSamplePos (Vertex* v) const ;

	/**
	 * get the old sample value that will be replaced to embed the bit represented by the vertex v
	 **/
	SampleValue *getOriginalSampleValue (Vertex *v) const ;

	/**
	 * get the sample value that should replace the previous sample value to embed the bit represented by the vertex v
	 **/
	SampleValue *getReplacingSampleValue (Vertex *v) const ;

	void print (unsigned short spc = 0) const ;
	
	private:
	Vertex *Vertex1 ;
	/// contains the index of the sample (of those in Vertex1) that will be changed (if this edge is used)
	unsigned short Index1 ;
	
	Vertex *Vertex2 ;
	/// contains the index of the sample (of those in Vertex2) that will be changed (if this edge is used)
	unsigned short Index2 ;

	UWORD32 Weight ;
} ;

#endif // ndef SH_EDGE_H

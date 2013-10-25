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

#ifndef SH_SAMPLEVALUEADJACENCYLIST_H
#define SH_SAMPLEVALUEADJACENCYLIST_H

#include <vector>

#include "SampleValue.h"
#include "common.h"

/**
 * \class SampleValueAdjacencyList
 * \brief an adjacency list-like data structur for sample values
 **/
class SampleValueAdjacencyList {
	public:
	/**
	 * construct a SampleValueAdjacencyList with numsvs rows
	 **/
	SampleValueAdjacencyList (SampleValueLabel numsvs) ;

	std::vector<SampleValue*>& operator[] (const SampleValueLabel lbl)
		{ return AdjacencyList[lbl] ; } ;

	std::vector<SampleValue*>& operator[] (const SampleValue* sv)
		{ return AdjacencyList[sv->getLabel()] ; } ;

	unsigned long getNumRows (void) const
		{ return AdjacencyList.size() ; } ;

	/**
	 * check if every row contains the same set of sample values
	 **/
	bool operator== (const SampleValueAdjacencyList& sval) ;

	/**
	 * sort the list in a way that the first entry of a row has the least distance to source sample value
	 **/
	void sort (void) ;

	private:
	std::vector<std::vector<SampleValue*> > AdjacencyList ;

	void quicksort (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r) ;
	/**
	 * partition oppneighs/distances into those with distances less than and those with distances greater than and equal to x
	 * \return the index in oppneighs/distances that separates the two
	 **/
	unsigned int partition (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r, UWORD32 x) ;
	void swap (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int i, unsigned int j) ;
} ;

#endif // ndef SH_SAMPLEVALUEADJACENCYLIST_H

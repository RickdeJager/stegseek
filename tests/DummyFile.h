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

#ifndef SH_DUMMYFILE_H
#define SH_DUMMYFILE_H

#include <vector>
#include <list>

class BitString ;
#include "CvrStgFile.h"
class Selector ;
#include "common.h"

/**
 * \class DummyFile
 * \brief a dummy CvrStgFile implementation to facilitate testing and debugging
 **/
class DummyFile : public CvrStgFile {
	public:
	/**
	 * construct a DummyFile object containing the sample values 0,1,...,s-1
	 * \param s the size of the DummyFile (i.e. the number of samples it should contain)
	 * \param svam the SampleValueAdjacencyMatrix for the Samples in this file
	 **/
	DummyFile (UWORD16 s, std::vector<std::vector<bool> >* svam) ;

	std::list<CvrStgFile::Property> getProperties (void) const ;

	unsigned long getNumSamples (void) const ;

	void replaceSample (const SamplePos pos, const SampleValue* s) ;

	SampleValue* getSampleValue (const SamplePos pos) const ;

	std::vector<std::vector<bool> >* getSampleValueAdjacencyMatrix() const
		{ return SampleValueAdjacencyMatrix ; } ;

	/**
	 * create a BitString, a DummyFile and a Selector that together will produce a graph like described by the adjacency list
	 * \param adjlist an adjacency list describing the "target graph"
	 * \param bs will be filled with the BitString
	 * \param f will be filled with the DummyFile
	 * \param s will be filled with the Selector
	 *
	 * Constructing a Graph object with "Graph (*f, **bs, **s)" will result in a graph of the form of adjlist.
	 *
	 * The constructed graph has the following form:
	 * SamplesPerVertex == 2, EmbValueModulus = 2
	 * every vertex has a sample value with bit == 0 at index 0 and one with bit == 1 at index 1,
	 * if two vertices are adjacent, one end of the edge is at index 0 of the vertex with
	 * the lower vertex label and the other end of the edge is at index 1 of the vertex with
	 * the higher vertex label.
	 * The distance between vertex with label i and vertex with label j is : 2*|j - i| + 1
	 **/
	static void createGraph (std::vector<std::list<UWORD16> >& adjlist, BitString** bs, CvrStgFile** f, Selector** s) ;

	private:
	static const unsigned short SamplesPerVertex = 2 ;
	static const EmbValue EmbValueModulus = 2 ;

	std::vector<UWORD16> Samples ;
	std::vector<std::vector<bool> >* SampleValueAdjacencyMatrix ;
} ;

#endif // ndef SH_DUMMYFILE_H

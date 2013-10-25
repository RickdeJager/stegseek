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

#ifndef SH_EDGEITERATOR_H
#define SH_EDGEITERATOR_H

#include <list>

#include "Edge.h"
class Graph ;
#include "SampleOccurence.h"
class SampleValue ;
class Vertex ;

/**
 * \class EdgeIterator
 * \brief allows an iteration trough all edges of a vertex
 *
 * The Vertex that is the source for all edges is called "source vertex".
 * The order of the iteration through the edges is from the shortest to the longest edge.
 * If two edges have the same length they are ordered the same way as the corresponding
 * entries in the sample value adjacency lists (for different sample values) respectivly
 * the destination sample occurences in the SampleOccurences data structure (for the same
 * sample value).
 *
 * EdgeIterator uses an SampleOccurence::const_iterator to store information
 * about the current edge. Graph::(un)markDeletedSampleOccurence can invalidate
 * such iterators. It is therefore not a good idea to use EdgeIterators at the same
 * time as the Graph::(un)markDeletedSampleOccurence functionality.
 *
 * <b>NOTE:</b> EdgeIterator relies on the Globals object pointed to by the Globs pointer.
 * This means that it must be set correctly before using any method of an EdgeIterator object.
 **/
class EdgeIterator {
	public:
	enum ITERATIONMODE {
		SAMPLEOCCURENCE,	// incrementing increments to next sample occurence (possibly of the same sample value) thus using every edge of the source vertex
		SAMPLEVALUE		// incrementing increments to the next sample value thus not using all edges in general
	} ;

	/**
	 * the default contructor - does not create a valid object
	 **/
	EdgeIterator (void) ;

	/**
	 * \param v the source vertex
	 **/
	EdgeIterator (Vertex *v, ITERATIONMODE m = SAMPLEOCCURENCE) ;

	/**
	 * the copy constructor
	 **/
	EdgeIterator (const EdgeIterator& eit) ;

	~EdgeIterator (void) ;

	/**
	 * get the current edge
	 * \return the edge that is described by the current status of this EdgeIterator
	 **/
	const Edge* operator* (void) const
		{ return ((Finished) ? NULL : &CurrentEdge) ; }

	/**
	 * set this iterator to next edge
	 **/
	void operator++ (void) ;

	/**
	 * set this iterator to first (shortest) edge of vertex v
	 * \param v new vertex (don't change if it is NULL)
	 **/
	void reset (Vertex* v, ITERATIONMODE m = SAMPLEOCCURENCE) ;

	/**
	 * reset this iterator to first (shortest) edge
	 **/
	void reset (ITERATIONMODE m = SAMPLEOCCURENCE) ;

	/**
	 * \return true iff this EdgeIterator points to the end of the list of edges of SrcVertex
	 **/
	bool isFinished (void) const
		{ return Finished ; } ;

	/**
	 * get the label of the partner vertex
	 * \return the label of the vertex that builds the edge returned by operator* together with SrcVertex
	 **/
	VertexLabel getPartnerVertexLabel (void) const
		{ return SampleOccurenceIt->getVertex()->getLabel() ; } ;

	static UWORD32 getMaxNumEdges (void)
		{ return MaxNumEdges ; } ;

	static void setMaxNumEdges (UWORD32 mne)
		{ MaxNumEdges = mne ; } ;

	void print (unsigned short spc = 0) const ;

	private:
	/// the current edge (is returned by operator*)
	Edge CurrentEdge ;

	/// mode of iteration
	ITERATIONMODE Mode ;

	/// contains (for every sample value) an index to the current opposite neighbour
	unsigned long* SVALIndices ;

	/// the maximum number of edges the EdgeIterator should iterate through
	static UWORD32 MaxNumEdges ;

	/// the index/number of the edge that is currently returned by operator*
	UWORD32 EdgeIndex ;

	/// is true iff there are no more edges for this source vertex
	bool Finished ;

	/**
	 * contains the iterator pointing to the sample occurence that constitutes
	 * the edge together with SourceVertex/SourceSamleValueIndex
	 **/
	std::list<SampleOccurence>::const_iterator SampleOccurenceIt ;

	/**
	 * find the shortest edge, starting the search at SVOppNeighsIndices[0...k]
	 * set the private variables accordingly
	 * is only called to find a new destination sample value, i.e. if one of the
	 * SVOppNeighsIndices[i] is changed
	 **/
	void findNextEdge (void) ;

	/**
	 * \return true iff there is a sample with value sv that is part of an edge starting at SrcVertex
	 **/
	bool isDestSampleValueOK (const SampleValue *sv) ;
} ;

#endif // ndef SH_EDGEITERATOR_H

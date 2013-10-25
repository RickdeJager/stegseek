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

#ifndef SH_VERTEX_H
#define SH_VERTEX_H

#include "CvrStgFile.h"
#include "common.h"

class Edge ;
class SampleOccurence ;
class SampleValue ;

/**
 * \class Vertex
 * \brief a vertex in a graph
 *
 * A vertex represents a bit that will cause a change to the cover-stego-file to be embedded.
 * A vertex consists of k samples (that is k sample values at k (different) positions in the
 * cover-stego-file), where k is TheCvrStgFile->getNumSamplesPerVertex(). One of these k samples
 * must be changed to an opposite sample to embed the bit that corresponds to this vertex.
 *
 * <b>NOTE:</b> Vertex relies on the Globals object pointed to by the Globs pointer.
 * This means that it must be set correctly before using any method of a Vertex object.
 **/
class Vertex {
	public:
	/**
	 * construct a new vertex object
	 * \param l the vertex label for this vertex
	 * \param sposs the array (with length g->getSamplesPerVertex()) of the positions of the samples
	 * \param svalues the array (with length g->getSamplesPerVertex()) of (unique (!)) pointers to the sample values
	 * \param t the target value for the whole vertex - the value that should be returned by getEmbeddedValue() after the embedding
	 **/
	Vertex (VertexLabel l, SamplePos* sposs, SampleValue** svalues, EmbValue t) ;

	~Vertex (void) ;

	/**
	 * get the i-th sample position
	 * \param i an index of a sample in this vertex (must be < TheCvrStgFile->getNumSamplesPerVertex())
	 * \return the position of the sample in the associated cvrstgfile
	 **/
	SamplePos getSamplePos (unsigned short i) const
		{ return SamplePositions[i] ; } ;

	/**
	 * get the i-th sample value
	 * \param i an index of a sample in this vertex (must be < TheCvrStgFile->getNumSamplesPerVertex())
	 * \return the value of the sample in the associated cvrstgfile
	 **/
	SampleValue* getSampleValue (unsigned short i) const
		{ return SampleValues[i] ; } ;

	/**
	 * get the degree of this vertex
	 **/
	UWORD32 getDegree (void) const ;

	/**
	 * get the shortest edge of this vertex
	 **/
	Edge *getShortestEdge (void) const
		{ return ShortestEdge ; } ;

	/**
	 * find shortest edge of this vertex and save result to ShortestEdge
	 **/
	void updateShortestEdge (void) ;

	/**
	 * if this vertex is valid, mark it as deleted
	 **/
	void markDeleted (void) ;

	/**
	 * if this vertex is marked as deleted, undo this
	 **/
	void unmarkDeleted (void) ;

	VertexLabel getLabel (void) const
		{ return Label ; } ;

	void setLabel (VertexLabel l)
		{ Label = l ; } ;

	void setSampleOccurenceIt (unsigned short i, std::list<SampleOccurence>::iterator it)
		{ SampleOccurenceIts[i] = it ; }

	EmbValue getEmbeddedValue (void) const ;

	EmbValue getTargetValue (unsigned short i) const
		{ return TargetValues[i] ; } ;

	void print (unsigned short spc = 0) const ;
	void printEdges (void) const ;

	private:
	/// the vertex label of this vertex
	VertexLabel Label ;

	/// the sample positions of the samples described by this vertex in the CvrStgFile
	SamplePos* SamplePositions ;

	/// the sample values at the SamplePositions
	SampleValue** SampleValues ;

	/// the target values for the sample values (exactly one of them has to be reached (and the other left unchanged) to embed this vertex)
	EmbValue* TargetValues ;

	/// point to entries in std::lists of sample occurences in the graph
	std::list<SampleOccurence>::iterator* SampleOccurenceIts ;

	/// the shortest edge of this vertex (as calculated by updateShortestEdge)
	Edge *ShortestEdge ;

	/// true iff this vertex is not deleted
	bool valid ;

	/// the number of loop edges (loop edges are not valid edges)
	unsigned short SelfDegree ;
} ;

#endif // ndef SH_VERTEX_H

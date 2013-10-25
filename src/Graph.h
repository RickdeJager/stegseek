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

#ifndef SH_GRAPH_H
#define SH_GRAPH_H

#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <vector>

#include "EdgeIterator.h"
#include "SampleValueAdjacencyList.h"
#include "common.h"
#include "wrapper_hash_set.h"

class BitString ;
class SampleOccurence ;
class Selector ;
class Vertex ;
class VertexContent ;
struct VertexContentsEqual ;

/**
 * \class Graph
 * \brief a graph constructed from a cover file and a message to be embedded
 *
 * This class provides a purely graph-theoretic interface to any other class.
 * Some classes however need access to the internal (steganographic) representation,
 * for example: Vertex, EdgeIterator,... . These are declared as friends of Graph here and
 * thus have direct access to the private data structures.
 **/
class Graph {
	public:
	/**
	 * construct a graph
	 * \param cvr the underlying cover file
	 * \param emb the bitstring to be embedded (with correct arity already set)
	 **/
	Graph (CvrStgFile* cvr, const BitString& emb, Selector& sel) ;

	/**
	 * destructor
	 **/
	~Graph (void) ;

	/**
	 * get the number of vertices in this graph
	 **/
	unsigned long getNumVertices (void) const
		{ return Vertices.size() ; } ;

	/**
	 * get a vertex
	 * \param l the vertex label (index) of the vertex to be returned (must be < getNumVertices())
	 * \return the vertex with label l
	 **/
	Vertex* getVertex (VertexLabel l) const
		{ return Vertices[l] ; } ;

	void unmarkDeletedAllVertices (void) ;

	/**
	 * calculate and return the average vertex degree
	 **/
	float getAvgVertexDegree (void) const ;

	void printVerboseInfo (void) ;

	/**
	 * check the integrity of all data structures,
	 * only used for debugging and testing
	 **/
	bool check (bool verbose = false) const ;
	/**
	 * check the integrity of the Vertices data structure,
	 * only used for debugging and testing
	 **/
	bool check_Vertices (bool verbose = false) const ;
	/**
	 * check the integrity of the SampleValues data structure,
	 * only used for debugging and testing
	 **/
	bool check_SampleValues (bool verbose = false) const ;
	/**
	 * check the integrity of the SampleOccurences data structure,
	 * it is assumed that DeletedSampleOccurences is empty,
	 * only used for debugging and testing
	 **/
	bool check_SampleOccurences (bool verbose = false) const ;
	/**
	 * check the integrity of the SVALists data structure,
	 * only used for debugging and testing
	 **/
	bool check_SVALists (bool verbose = false) const ;

#ifdef DEBUG
	/**
	 * prints graph in a format suitable as input to the C implementation
	 * of Gabow's non-weighted matching algorithm by E. Rothberg to stdout
	 * (available at: ftp://ftp.zib.de/pub/Packages/mathprog/matching/index.html
	 **/
	void print (void) const ;

	void print_gml (std::ostream& out) const ;
	void printVertex_gml (std::ostream& out, Vertex* v, unsigned int recdepth, std::vector<bool>& nodeprinted, std::vector<bool>& edgesprinted, bool start = true) const ;
	void printPrologue_gml (std::ostream& out) const ;
	void printEpilogue_gml (std::ostream& out) const ;

	void print_Vertices (unsigned short spc = 0) const ;
#endif

	private:
	//
	// friend-declarations
	//
	friend class WKSConstructionHeuristic ;
	friend class EdgeIterator ;
	friend class SampleValueAdjacencyList ;
	friend class Vertex ;

	/// contains the vertices in this graph - Vertices[l] is the vertex with label l
	std::vector<Vertex*> Vertices ;

	/// contains the list of (unique) sample values - SampleValues[l] is the sample value with label l
	std::vector<SampleValue*> SampleValues ;

	/// contains the sample value adjacency lists (SVALists[v] contains only sample values with embedded value v)
	std::vector<SampleValueAdjacencyList*> SVALists ;

	/// SampleOccurences[l] contains all occurences of the sample value with label l
	std::vector<std::list<SampleOccurence> > SampleOccurences ;

	/**
	 * NumSampleOccurences[l][t] contains the number vertices that contain the sample value with label l and associated target t
	 **/
	std::vector<UWORD32*> NumSampleOccurences ;

	/// contains those sample occurences that have been marked as deleted from SampleOccurences
	std::vector<std::list<SampleOccurence> > DeletedSampleOccurences ;
	std::vector<UWORD32*> NumDeletedSampleOccurences ;

	std::list<SampleOccurence>::iterator markDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;
	std::list<SampleOccurence>::iterator unmarkDeletedSampleOccurence (std::list<SampleOccurence>::iterator it) ;

	//
	// end of friend-declarations
	// Note: private members of Graph that are declared beyond this point should
	//       not be used by friends.
	//

	/**
	 * construct sample-related data structures
	 *
	 * needs: sposs(unsorted)
	 * provides: svalues(unsorted,unique), SampleValues
	 **/
	void constructSamples (const std::vector<SamplePos*> &sposs, std::vector<SampleValue**>& svalues) ;

	/**
	 * construct vertex-related data structures
	 *
	 * needs: sposs(unsorted), svalues(unsorted,unique), tvalues
	 * provides: sposs(sorted), Vertices (except SampleOccurenceIts)
	 **/
	void constructVertices (std::vector<SamplePos*>& sposs, std::vector<SampleValue**>& svalues, const std::vector<EmbValue>& tvalues) ;

	/**
	 * construct edge-related data structures
	 *
	 * needs: SampleValues, Vertices (except SampleOccurenceIts)
	 * provides: SVALists, SampleOccurences, Vertices (SampleOccurenceIts)
	 **/
	void constructEdges (void) ;

	CvrStgFile *File ;
	EmbValue EmbValueModulus ;
	unsigned short SamplesPerVertex ;

	bool check_SampleOccurences_size (bool verbose = false) const ;
	bool check_SampleOccurences_correctness (bool verbose = false) const ;
	bool check_SampleOccurences_completeness (bool verbose = false) const ;

	bool check_SVALists_size (bool verbose = false) const ;
	bool check_SVALists_soundness (bool verbose = false) const ;
	bool check_SVALists_sorted (bool verbose = false) const ;
	bool check_SVALists_uniqueness (bool verbose = false) const ;
	bool check_SVALists_completeness (bool verbose = false) const ;
} ;

#endif // ndef SH_GRAPH_H

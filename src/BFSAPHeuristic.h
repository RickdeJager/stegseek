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

#ifndef SH_BFSAPHeuristic
#define SH_BFSAPHeuristic

#include "EdgeIterator.h"
#include "MatchingAlgorithm.h"
#include "Vertex.h"

class Edge ;
#include "Graph.h"
class Matching ;

/**
 * \class BFSAPHeuristic
 * \brief a matching algorithm implementing a heuristic breadth-first-search for augmenting paths
 **/
class BFSAPHeuristic : public MatchingAlgorithm {
	public:
	/**
	 * construct an BFSAPHeuristic object
	 * \param g the graph on which this heuristic should run
	 * \param m the matching to start with
	 **/
	BFSAPHeuristic (Graph* g, Matching* m) ;

	virtual ~BFSAPHeuristic (void) ;

	const char* getName (void) const
		{ return "BFS Augmenting Path Heuristic" ; } ;

	void run (void) ;

	private:
	/**
	 * \param v0 an exposed vertex
	 * \param path an array of Edge pointers where the path will be put
	 * \return the length of the path (the number of valid edges in path)
	 **/
	unsigned long searchAugmentingPath (Vertex* v0, const Edge** path) ;

	bool* VertexVisited ;
	Edge* BackEdge ;
} ;

#endif // ndef SH_BFSAPHeuristic

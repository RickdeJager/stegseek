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

#include "BFSAPHeuristic.h"
#include "Edge.h"
#include "EdgeIterator.h"
#include "Graph.h"
#include "Matching.h"
#include "common.h"

BFSAPHeuristic::BFSAPHeuristic (Graph* g, Matching* m)
	: MatchingAlgorithm (g, m, 100.0)
{
	unsigned long numvertices = g->getNumVertices() ;

	VertexVisited = new bool[numvertices] ;
	BackEdge = new Edge[numvertices] ;
	for (VertexLabel l = 0 ; l < numvertices ; l++) {
		VertexVisited[l] = false ;
	}
}

BFSAPHeuristic::~BFSAPHeuristic ()
{
	delete[] VertexVisited ;
	delete[] BackEdge ;
}

void BFSAPHeuristic::run ()
{
	const Edge** path = new const Edge*[TheGraph->getNumVertices()] ;

	const std::list<Vertex*> ExposedVertices = TheMatching->getExposedVertices() ;
	for (std::list<Vertex*>::const_iterator expv = ExposedVertices.begin() ;
		(expv != ExposedVertices.end()) && (TheMatching->getCardinality() < CardinalityGoal) ; expv++) {
		if (TheMatching->isExposed (*expv)) {
			unsigned long pathlength = searchAugmentingPath (*expv, path) ;

#ifdef DEBUG
			if (pathlength == 0) {
				printDebug (5, "BFSAPHeuristic: could not find augmenting path for vertex %lu", (*expv)->getLabel()) ;
			}
			else {
				if (RUNDEBUGLEVEL(5)) {
					std::cerr << "BFSAPHeuristic: found augmenting path for vertex " << (*expv)->getLabel() << ": " ;
					for (unsigned long i = 0 ; i < pathlength ; i++) {
						std::cerr << path[i]->getVertex1()->getLabel() << "-" << path[i]->getVertex2()->getLabel() ;
						if (i != pathlength - 1) {
							std::cerr << ", " ;
						}
					}
					std::cerr << std::endl ;
				}
			}
#endif
			if (pathlength > 0) {
				TheMatching->augment ((const Edge**) path, pathlength) ;
			}
		}
	}

	delete[] path ;
}

unsigned long BFSAPHeuristic::searchAugmentingPath (Vertex *v0, const Edge** path)
{
#ifdef DEBUG
	printDebug (5, "BFSAPHeuristic: starting search for augmenting path from vertex %lu", v0->getLabel()) ;
#endif

	unsigned long pathlen = 0 ;
	bool foundap = false ;

	std::list<Vertex*> srclist ;
	std::list<Vertex*> destlist ;
	srclist.push_back (v0) ;
	VertexVisited[v0->getLabel()] = true ;

	for (unsigned int depth = 0 ; !foundap && !srclist.empty() ; depth++) {
		myassert (destlist.empty()) ;

		// for all vertices v1 in srclist...
		for (std::list<Vertex*>::const_iterator vit = srclist.begin() ; vit != srclist.end() && !foundap ; vit++) {
			Vertex* v1 = *vit ;
#ifdef DEBUG
			printDebug (6, "BFSAPHeuristic: starting search iteration from vertex %lu", v1->getLabel()) ;
#endif

			// ...iterate through all...
			EdgeIterator edgeit (v1) ;
			while (!edgeit.isFinished() && !foundap) {
				Edge e = **edgeit ;

				// ...unmatched edges of v1
				if (!TheMatching->includesEdge(e)) {
					Vertex* v2 = e.getOtherVertex(v1) ;
#ifdef DEBUG
					printDebug (7, "BFSAPHeuristic: examining unmatched edge %lu - %lu", v1->getLabel(), v2->getLabel()) ;
#endif

					if (!VertexVisited[v2->getLabel()]) { // v2 is not yet visited -> visit it!
						VertexVisited[v2->getLabel()] = true ;
						BackEdge[v2->getLabel()] = e ;

						if (TheMatching->isExposed(v2)) { // an augmenting path has been found!
							foundap = true ;
							pathlen = 2 * depth + 1 ;
							// build path backwards
							Vertex *v = v2 ;
							for (int j = pathlen - 1 ; j >= 0 ; j--) {
								path[j] = &BackEdge[v->getLabel()] ;
								v = BackEdge[v->getLabel()].getOtherVertex(v) ;
							}
						}
						else { // continue searching
							Edge me = *(TheMatching->getMatchingEdge(v2)) ;
							Vertex* v3 = me.getOtherVertex(v2) ;
							BackEdge[v3->getLabel()] = me ;
							destlist.push_back (v3) ; // use v3 as a v1 in next iteration
							VertexVisited[v3->getLabel()] = true ;
						}
					}
				}

				++edgeit ;
			}
		}

		if (!foundap) {
			// prepare for next iteration
			srclist.clear() ;
			srclist.splice (srclist.begin(), destlist) ;
		}
	}

	return pathlen ;
}

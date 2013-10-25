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

#include "WKSConstructionHeuristic.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "common.h"

WKSConstructionHeuristic::WKSConstructionHeuristic (Graph* g, Matching* m, float goal)
	: MatchingAlgorithm (g, m, goal)
{
	unsigned long nvertices = g->getNumVertices() ;

	// fill the VerticesDeg1 and VerticesDegG priority queues
	VerticesDeg1 = std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> () ;
	VerticesDegG = std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge> () ;

	for (VertexLabel l = 0 ; l < nvertices ; l++) {	
		Vertex *v = g->getVertex(l) ;
		v->updateShortestEdge() ;
		UWORD32 degree = v->getDegree() ;
		if (degree != 0) {
			myassert (v->getShortestEdge() != NULL) ;
			if (degree == 1) {
				VerticesDeg1.push (v) ;
			}
			else {
				VerticesDegG.push (v) ;
			}
		}
	}
}

void WKSConstructionHeuristic::run ()
{
	unsigned long numDegG = 0 ;
	unsigned long numDeg1 = 0 ;

	while ((TheMatching->getCardinality() < CardinalityGoal) && !(VerticesDegG.empty() && VerticesDeg1.empty())) {
		Vertex *v = NULL ;

		// get a vertex from one of the priority queues
		if (!VerticesDeg1.empty()) {
			v = findVertexDeg1() ;
			if (v != NULL) {
#ifdef DEBUG
				printDebug (5, "WKSConstructionHeuristic: vertex %lu has been chosen from VerticesDeg1.", v->getLabel()) ;
#endif
				numDeg1++ ;
			}
		}
		else {
			v = findVertexDegG() ;
			if (v != NULL) {
#ifdef DEBUG
				printDebug (5, "WKSConstructionHeuristic: vertex %lu has been chosen from VerticesDegG.", v->getLabel()) ;
#endif
				numDegG++ ;
			}
		}

		if (v != NULL) { // insert v's shortest edge into matching
			Edge* e = v->getShortestEdge() ;
			Vertex* vother = e->getOtherVertex(v) ;

#ifdef DEBUG
			printDebug (5, "WKSConstructionHeuristic: inserting edge %lu - %lu into matching", v->getLabel(), vother->getLabel()) ;
#endif
			TheMatching->addEdge(*e) ;
			v->markDeleted() ;
			vother->markDeleted() ;
			checkNeighboursDeg1 (v) ;
			checkNeighboursDeg1 (vother) ;
		}
	}

	TheGraph->unmarkDeletedAllVertices() ;
}

void WKSConstructionHeuristic::checkNeighboursDeg1 (Vertex *v)
{
	// for all sample values in v...
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SampleValue* srcsv = v->getSampleValue(i) ;
		const std::vector<SampleValue*>& tneighbours = (*(TheGraph->SVALists[v->getTargetValue(i)]))[srcsv] ;

		// ...get all target neighbour sample values...
		for (std::vector<SampleValue*>::const_iterator tnit = tneighbours.begin() ; tnit != tneighbours.end() ; tnit++) {
			const std::list<SampleOccurence>& soccs = TheGraph->SampleOccurences[(*tnit)->getLabel()] ;

			// ...and for each of them look at every sample occurence of it,...
			for (std::list<SampleOccurence>::const_iterator soccit = soccs.begin() ; soccit != soccs.end() ; soccit++) {
				// ...determine wether it is in an edge with v and ...
				if (srcsv->getEmbeddedValue() == soccit->getVertex()->getTargetValue(soccit->getIndex())) {
					// ...if the corresponding vertex has degree 1:
					if (soccit->getVertex()->getDegree() == 1) {
						soccit->getVertex()->updateShortestEdge() ;
						VerticesDeg1.push(soccit->getVertex()) ; // move it to the Deg1 priority queue
					}
				}
			}
		}
	}
}

Vertex* WKSConstructionHeuristic::findVertexDegG ()
{
	Vertex *v = NULL ;
	bool usethisvertex = false ;

	// get the vertex that is nearest to top (with updated len of shortest edge) and has degree > 1
	do {
		myassert (!VerticesDegG.empty()) ;
		v = VerticesDegG.top() ;
		VerticesDegG.pop() ;

		if ((TheMatching->isExposed(v)) && (v->getDegree() > 1)) { // implicitly delete vertices that have been moved to VerticesDeg1 or have already been matched
			UWORD32 weight_before = v->getShortestEdge()->getWeight() ;
			
			v->updateShortestEdge() ;

			if (v->getShortestEdge()->getWeight() == weight_before) {
				usethisvertex = true ;
			}
			else {
				myassert (v->getShortestEdge()->getWeight() > weight_before) ;	// weight can only rise
				VerticesDegG.push (v) ; // push v into a position that is further away from top
			}
		}
	} while (!(usethisvertex || VerticesDegG.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

Vertex *WKSConstructionHeuristic::findVertexDeg1 ()
{
	Vertex *v = NULL ;
	bool usethisvertex = false ;

	// get the vertex that is nearest to top and still has degree 1
	do {
		myassert (!VerticesDeg1.empty()) ;
		v = VerticesDeg1.top() ;
		VerticesDeg1.pop() ;

		if ((TheMatching->isExposed(v)) && (v->getDegree() == 1)) { // implicitly delete vertices that have degree zero or have already been matched
			usethisvertex = true ;
		}
	} while (!(usethisvertex || VerticesDeg1.empty())) ;

	return (usethisvertex ? v : NULL) ;
}

bool WKSConstructionHeuristic::LongerShortestEdge::operator() (const Vertex* v1, const Vertex* v2)
{
	bool retval = false ;
	UWORD32 deg1 = v1->getDegree() ;
	UWORD32 deg2 = v2->getDegree() ;
	if (deg1 == 0 && deg2 == 0) {
		retval = (v1->getLabel() > v2->getLabel()) ;
	}
	else if (deg1 == 0) {
		retval = true ;
	}
	else if (deg2 == 0) {
		retval = false ;
	}
	else {
		retval = v1->getShortestEdge()->getWeight() > v2->getShortestEdge()->getWeight();
	}
	return retval ;
}

#ifdef DEBUG
void WKSConstructionHeuristic::print (unsigned short spc)
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "VerticesDeg1 (top->down): " ;
	printPQ (VerticesDeg1) ;
	std::cerr << std::endl ;

	std::cerr << space << "VerticesDegG (top->down): " ;
	printPQ (VerticesDegG) ;
	std::cerr << std::endl ;
}

void WKSConstructionHeuristic::printPQ (std::priority_queue<Vertex*, std::vector<Vertex*>, LongerShortestEdge>& pq)
{
	std::vector<Vertex*> backup ;
	while (!pq.empty()) {
		Vertex *v = pq.top() ;
		pq.pop() ;
		std::cerr << v->getLabel() << ", " ;
		backup.push_back(v) ;
	}

	for (std::vector<Vertex*>::const_iterator it = backup.begin() ; it != backup.end() ; it++) {
		pq.push (*it) ;
	}
}
#endif

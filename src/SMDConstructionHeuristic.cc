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

#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "SMDConstructionHeuristic.h"
#include "Vertex.h"

SMDConstructionHeuristic::SMDConstructionHeuristic (Graph* g, Matching* m, float goal)
	: MatchingAlgorithm (g, m, goal)
{
	Vertices = std::vector<Vertex*> (g->getNumVertices()) ;
	for (VertexLabel l = 0 ; l < g->getNumVertices() ; l++) {
		Vertices[l] = g->getVertex(l) ;
		myassert (m->isExposed (Vertices[l])) ;
	}

	sort (Vertices.begin(), Vertices.end(), SmallerVertexDegree()) ;
}

void SMDConstructionHeuristic::run (void)
{
	for (VertexLabel l = 0 ; l < Vertices.size() ; l++) {
		if (TheMatching->isExposed(Vertices[l]) && Vertices[l]->getDegree() > 0) {
			Vertex* v1 = Vertices[l] ;
			v1->updateShortestEdge() ;
			Edge* e = v1->getShortestEdge() ;
			Vertex* v2 = e->getOtherVertex (v1) ;

			TheMatching->addEdge(*e) ;

			v1->markDeleted() ;
			v2->markDeleted() ;
		}
	}

	TheGraph->unmarkDeletedAllVertices() ;
}

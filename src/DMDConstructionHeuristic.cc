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

#include "DMDConstructionHeuristic.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "Vertex.h"

DMDConstructionHeuristic::DMDConstructionHeuristic (Graph* g, Matching* m, float goal)
	: MatchingAlgorithm (g, m, goal)
{
	AvailableVertices = std::vector<Vertex*> (g->getNumVertices()) ;
	for (VertexLabel l = 0 ; l < g->getNumVertices() ; l++) {
		AvailableVertices[l] = g->getVertex(l) ;
	}
}

void DMDConstructionHeuristic::run (void)
{
	VertexLabel mdi = MinDegNotFound ;
	while ((mdi = findMinDegIndex (AvailableVertices)) != MinDegNotFound) {
		Vertex* v1 = AvailableVertices[mdi] ;
		v1->updateShortestEdge() ;
		Edge* e = v1->getShortestEdge() ;
		Vertex* v2 = e->getOtherVertex(v1) ;

		TheMatching->addEdge(*e) ;

		v1->markDeleted() ;
		v2->markDeleted() ;
		AvailableVertices[v1->getLabel()] = NULL ;
		AvailableVertices[v2->getLabel()] = NULL ;
	}

	TheGraph->unmarkDeletedAllVertices() ;
}

VertexLabel DMDConstructionHeuristic::findMinDegIndex (const std::vector<Vertex*>& vertices)
{
	VertexLabel mdi = MinDegNotFound ;
	UWORD32 mindeg = UWORD32_MAX ;
	for (VertexLabel l = 0 ; l < vertices.size() ; l++) {
		if (vertices[l] != NULL) {
			UWORD32 curdeg = vertices[l]->getDegree() ;
			if (0 < curdeg && curdeg < mindeg) {
				mdi = l ;
				mindeg = curdeg ;
			}
		}
	}
	return mdi ;
}

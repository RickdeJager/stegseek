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
#include "ProgressOutput.h"
#include "common.h"
#include "msg.h"

Matching::Matching (Graph* g, ProgressOutput* po)
	: TheGraph(g), PrOut(po)
{
	unsigned long nvertices = TheGraph->getNumVertices() ;
	for (unsigned long i = 0 ; i < nvertices ; i++) {
		ExposedVertices.push_back (TheGraph->getVertex (i)) ;
	}

	VertexInformation.reserve (nvertices) ;
	for (std::list<Vertex*>::iterator i = ExposedVertices.begin() ; i != ExposedVertices.end() ; i++) {
		VertexInformation.push_back (VertexInfo (i)) ;
	}

	Cardinality = 0 ;
}

Matching::~Matching ()
{
	for (std::list<Edge*>::iterator edgeit = MatchingEdges.begin() ; edgeit != MatchingEdges.end() ; edgeit++) {
		delete (*edgeit) ;
	}
}

bool Matching::includesEdge (const Edge& e) const
{
	Vertex* v1 = e.getVertex1() ;
	Vertex* v2 = e.getVertex2() ;
	bool v1ok = false, v2ok = false ;

	if (VertexInformation[v1->getLabel()].isMatched()) {
		if (*(VertexInformation[v1->getLabel()].getMatchingEdge()) == e) {
			v1ok = true ;
		}
	}
	if (VertexInformation[v2->getLabel()].isMatched()) {
		if (*(VertexInformation[v2->getLabel()].getMatchingEdge()) == e) {
			v2ok = true ;
		}
	}
	return (v1ok && v2ok) ;
}

void Matching::addEdge (const Edge& e)
{
	VertexLabel vlbl1 = e.getVertex1()->getLabel() ;
	VertexLabel vlbl2 = e.getVertex2()->getLabel() ;

	myassert (VertexInformation[vlbl1].isExposed()) ;
	myassert (VertexInformation[vlbl2].isExposed()) ;
	std::list<Edge*>::iterator edgeit = MatchingEdges.insert (MatchingEdges.end(), new Edge (e)) ;

	ExposedVertices.erase (VertexInformation[vlbl1].getExposedIterator()) ;
	ExposedVertices.erase (VertexInformation[vlbl2].getExposedIterator()) ;
	VertexInformation[vlbl1].setMatched (edgeit) ;
	VertexInformation[vlbl2].setMatched (edgeit) ;

	setCardinality (Cardinality + 1) ;
}

void Matching::removeEdge (const Edge& e)
{
	Vertex* v1 = e.getVertex1() ;
	Vertex* v2 = e.getVertex2() ;
	VertexLabel vlbl1 = v1->getLabel() ;
	VertexLabel vlbl2 = v2->getLabel() ;

	// assert: matching contains e
	myassert (VertexInformation[vlbl1].isMatched()) ;
	myassert (VertexInformation[vlbl2].isMatched()) ;

	// delete e from MatchingEdges
	std::list<Edge*>::iterator eit1 = VertexInformation[vlbl1].getMatchedIterator() ;
	std::list<Edge*>::iterator eit2 = VertexInformation[vlbl2].getMatchedIterator() ;
	myassert (eit1 == eit2) ;
	delete (*eit1) ;
	MatchingEdges.erase (eit1) ;

	// add v1,v2 to ExposedVertices
	std::list<Vertex*>::iterator expvit1 = ExposedVertices.insert (ExposedVertices.end(), v1) ;
	VertexInformation[vlbl1].setExposed (expvit1) ;
	std::list<Vertex*>::iterator expvit2 = ExposedVertices.insert (ExposedVertices.end(), v2) ;
	VertexInformation[vlbl2].setExposed (expvit2) ;

	setCardinality (Cardinality - 1) ;
}

Matching& Matching::augment (const Edge** path, unsigned long len)
{
	// TODO - rewrite this to use structure like: while (+--+==+ -> +==+--+); +--+ -> +==+ where == is matched edge
	myassert (len % 2 == 1) ;
	bool e_was_matched = false ;
	Edge *e = NULL ;
	Edge *e_before = NULL ;
	for (unsigned int i = 0 ; i < len ; i++) {
		// give e the correct orientation (using pointer equivalence of vertices(!))
		e = new Edge (*(path[i])) ;
		if (e_before == NULL) {
			if (len > 1) {
				// e is the first, but not the only edge in path
				if ((e->getVertex1() == path[1]->getVertex1()) || (e->getVertex1() == path[1]->getVertex2())) {
					e->swap() ;
				}
			}
		}
		else {
			if (e->getVertex1() != e_before->getVertex2()) {
				e->swap() ;
			}
			myassert (e->getVertex1() == e_before->getVertex2()) ;
		}

		// make changes in VertexInformation, ExposedVertices and MatchingEdges
		if (e_was_matched) { // at the time this is called, v1 is matched with "v0"
			Vertex *v2 = e->getVertex2() ;
			VertexLabel v2lbl = v2->getLabel() ;

			// remove old edge from matching
			delete (*(VertexInformation[v2lbl].getMatchedIterator())) ;
			MatchingEdges.erase (VertexInformation[v2lbl].getMatchedIterator()) ;

			// v2 is exposed now (for one iteration)
			std::list<Vertex*>::iterator expvit2 = ExposedVertices.insert (ExposedVertices.end(), v2) ;
			VertexInformation[v2lbl].setExposed (expvit2) ;
		}
		else {
			Vertex *v1 = e->getVertex1() ;
			Vertex *v2 = e->getVertex2() ;
			VertexLabel v1lbl = v1->getLabel() ;
			VertexLabel v2lbl = v2->getLabel() ;
		
			// v1 is no longer exposed
			ExposedVertices.erase (VertexInformation[v1lbl].getExposedIterator()) ;

			// add new edge to matching
			std::list<Edge*>::iterator edgeit = MatchingEdges.insert (MatchingEdges.end(), e) ;
			VertexInformation[v1lbl].setMatched (edgeit) ;
			VertexInformation[v2lbl].setMatched (edgeit) ;
		}

		e_was_matched = !e_was_matched ;
		e_before = e ;
	}

	ExposedVertices.erase (VertexInformation[e->getVertex2()->getLabel()].getExposedIterator()) ;
	VertexInformation[e->getVertex2()->getLabel()].setMatched (find (MatchingEdges.begin(), MatchingEdges.end(), e)) ;

	setCardinality (Cardinality + 1) ;

	return *this ;
}

Matching& Matching::augment (const std::vector<Edge*>& path)
{
	unsigned long len = path.size() ;
	const Edge** p = new const Edge*[len] ;
	for (unsigned long i = 0 ; i < len ; i++) {
		p[i] = path[i] ;
	}
	return augment (p, len) ;
}

void Matching::printVerboseInfo (void) const
{
	// only used for STATS output

	if (Args.Verbosity.getValue() == STATS) {
		float sumweights = 0 ;
		for (std::list<Edge*>::const_iterator it = MatchingEdges.begin() ; it != MatchingEdges.end() ; it++) {
			sumweights += (*it)->getWeight() ;
		}

		printf ("%.4f:%.1f:",
			1.0 - (((float) (getCardinality() * 2)) / ((float) TheGraph->getNumVertices())),  // percentage of unmatched vertices
			((float) sumweights / (float) getCardinality()) // average edge weight
		) ;
	}
}

float Matching::getMatchedRate (void) const
{
	return (((float) (2 * Cardinality)) / ((float) TheGraph->getNumVertices())) ;
}

float Matching::getAvgEdgeWeight (void) const
{
	float sumweights = 0 ;
	for (std::list<Edge*>::const_iterator it = MatchingEdges.begin() ; it != MatchingEdges.end() ; it++) {
		sumweights += (*it)->getWeight() ;
	}
	return (sumweights / (float) getCardinality()) ;
}

void Matching::setCardinality (unsigned long c)
{
	Cardinality = c ;
	if (PrOut) {
		PrOut->update (getMatchedRate()) ;
	}
}

bool Matching::check () const
{
	bool retval = true ;
	retval = check_MatchingEdges_vs_VertexInformation() && retval ;
	retval = check_ExposedVertices_vs_VertexInformation() && retval ;
	retval = check_VertexInformation_Integrity() && retval ;
	return retval ;
}

bool Matching::check_MatchingEdges_vs_VertexInformation () const
{
	bool err = false ;
	// for every e = (v1,v2) in MatchingEdges: isMatched(v1) && isMatched(v2)
	for (std::list<Edge*>::const_iterator it = MatchingEdges.begin() ; it != MatchingEdges.end() ; it++) {
		Vertex *v1 = (*it)->getVertex1() ;
		Vertex *v2 = (*it)->getVertex2() ;
		if (VertexInformation[v1->getLabel()].isExposed() || VertexInformation[v2->getLabel()].isExposed()) {
			err = true ;
			break ;
		}
	}

	if (err) {
		std::cerr << "FAILED: There is an edge in MatchingEdges that is adjacent to a vertex marked as exposed." << std::endl ;
	}

	return !err ;
}

bool Matching::check_ExposedVertices_vs_VertexInformation () const
{
	bool err = false ;

	// for every exposed vertex v: isExposed(v)
	for (std::list<Vertex*>::const_iterator it = ExposedVertices.begin() ; it != ExposedVertices.end() ; it++) {
		if (VertexInformation[(*it)->getLabel()].isMatched()) {
			err = true ;
			break ;
		}
	}

	if (err) {
		std::cerr << "FAILED: There is a vertex in ExposedVertices that is marked matched." << std::endl ;
	}

	return !err ;
}

bool Matching::check_VertexInformation_Integrity () const
{
	bool err_matched = false ;

	for (unsigned long vlbl = 0 ; vlbl < VertexInformation.size() ; vlbl++) {
		if (VertexInformation[vlbl].isMatched()) {
			Edge* e = *(VertexInformation[vlbl].getMatchedIterator()) ;
			if ((e->getVertex1()->getLabel() != vlbl) && (e->getVertex2()->getLabel() != vlbl)) {
				if (vlbl == 16) { std::cerr << "FAILED, printing edge:" << std::endl ; e->print() ; }
				err_matched = true ;
				break ;
			}
		}
	}

	if (err_matched)  {
		std::cerr << "FAILED: There is a shortest edge that is not adjacent to its vertex." << std::endl ;
	}

	return !err_matched ;
}

bool Matching::check_ValidAugPath (const std::vector<Edge*>& path) const
{
	// check cohesion
	bool cohesion = true ;
	std::vector<Vertex*> vertices ;
	Vertex* lastvertex = NULL ;
	if (path[1]->contains (path[0]->getVertex1())) {
		vertices.push_back (path[0]->getVertex2()) ;
		vertices.push_back (path[0]->getVertex1()) ;
		lastvertex = path[0]->getVertex1() ;
	}
	else if (path[1]->contains (path[0]->getVertex2())) {
		vertices.push_back (path[0]->getVertex1()) ;
		vertices.push_back (path[0]->getVertex2()) ;
		lastvertex = path[0]->getVertex2() ;
	}
	else {
		cohesion = false ;
	}

	std::vector<Edge*>::const_iterator it = path.begin() ;
	for (it++ ; it != path.end() ; it++) {
		if ((*it)->getVertex1() == lastvertex) {
			vertices.push_back ((*it)->getVertex2()) ;
			lastvertex = (*it)->getVertex2() ;
		}
		else if ((*it)->getVertex2() == lastvertex) {
			vertices.push_back ((*it)->getVertex1()) ;
			lastvertex = (*it)->getVertex1() ;
		}
		else {
			cohesion = false ;
		}
	}

	myassert (vertices.size() - 1 == path.size()) ;

	// check that path has no loop
	bool hasloop = false ;
	unsigned long nvertices = vertices.size() ;
	for (unsigned long i = 0 ; i < nvertices ; i++) {
		for (unsigned long j = i + 1 ; j < nvertices ; j++) {
			if (vertices[i] == vertices[j]) {
				hasloop = true ;
			}
		}
	}

	// check that path is augmenting w.r.t. the matching
	bool isaugmenting = true ;
	bool shouldinclude = false ;
	for (std::vector<Edge*>::const_iterator it = path.begin() ; it != path.end() ; it++) {
		if (shouldinclude) {
			isaugmenting = includesEdge(*it) && isaugmenting ;
		}
		else {
			isaugmenting = !includesEdge(*it) && isaugmenting ;
		}
		shouldinclude = !shouldinclude ;
	}

	return cohesion && !hasloop && isaugmenting ;
}

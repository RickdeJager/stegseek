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

#include "BitString.h"
#include "Edge.h"
#include "Graph.h"
#include "Selector.h"

#include "DummyFile.h"
#include "MatchingTest.h"

#define CREATEEDGEPTR(G,V1,V2) (new Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1))
#define CREATEEDGEOBJ(G,V1,V2) (Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1))

MatchingTest::MatchingTest (TestSuite* s)
	: UnitTest ("Matching", s)
{
	ADDTESTCATEGORY (MatchingTest, testAddRemoveEdge) ;
	ADDTESTCATEGORY (MatchingTest, testAugmentingPath) ;
}

void MatchingTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	std::vector<std::list<UWORD16> > adjlist1 (4) ;
	adjlist1[0].push_back(1) ; adjlist1[0].push_back(2) ;
	adjlist1[1].push_back(2) ; adjlist1[1].push_back(3) ;
	DummyFile::createGraph (adjlist1, &bs1, &f1, &s1) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	m1 = new Matching (g1) ;
	gl1 = Globs ;

	Globs.reset() ;
	std::vector<std::list<UWORD16> > adjlist2 (10) ;
	adjlist2[0].push_back(1) ; adjlist2[0].push_back(4) ;
	adjlist2[1].push_back(2) ; adjlist2[1].push_back(3) ; adjlist2[1].push_back(7) ;
	adjlist2[2].push_back(3) ; adjlist2[2].push_back(4) ; adjlist2[2].push_back(5) ; adjlist2[2].push_back(6) ;
	adjlist2[3].push_back(6) ;
	adjlist2[4].push_back(6) ; adjlist2[4].push_back(7) ; adjlist2[4].push_back(8) ; adjlist2[4].push_back(9) ;
	adjlist2[5].push_back(6) ;
	adjlist2[7].push_back(8) ;
	DummyFile::createGraph (adjlist2, &bs2, &f2, &s2) ;
	g2 = new Graph (f2, *bs2, *s2) ;
	m2 = new Matching (g2) ;
	gl2 = Globs ;
}

void MatchingTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete m1 ;
	delete g1 ;
	delete s1 ;
	delete bs1 ;
	delete f1 ;

	delete m2 ;
	delete g2 ;
	delete s2 ;
	delete bs2 ;
	delete f2 ;
}

void MatchingTest::testAddRemoveEdge ()
{
	{ // test Matching m1
		Globs = gl1 ;

		Edge e1 = CREATEEDGEOBJ (g1, 0, 2) ;
		m1->addEdge (e1) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isExposed((VertexLabel) 1) &&
						m1->isMatched((VertexLabel) 2) &&
						m1->isExposed((VertexLabel) 3)) ;

		Edge e2 = CREATEEDGEOBJ (g1, 1, 3) ;
		m1->addEdge (e2) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isMatched((VertexLabel) 2) &&
						m1->isMatched((VertexLabel) 3)) ;

		m1->removeEdge (e1) ;
		addTestResult (	m1->isExposed((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isExposed((VertexLabel) 2) &&
						m1->isMatched((VertexLabel) 3)) ;

		m1->removeEdge (e2) ;
		addTestResult (	m1->isExposed((VertexLabel) 0) &&
						m1->isExposed((VertexLabel) 1) &&
						m1->isExposed((VertexLabel) 2) &&
						m1->isExposed((VertexLabel) 3)) ;

	}

	{ // test Matching m2
		Globs = gl2 ;

		Edge e1 = CREATEEDGEOBJ (g2, 1, 7) ;
		m2->addEdge (e1) ;
		addTestResult (	m2->getCardinality() == 1 &&
						m2->includesEdge(e1)) ;

		Edge e2 = CREATEEDGEOBJ (g2, 4, 8) ;
		m2->addEdge (e2) ;
		addTestResult (	m2->getCardinality() == 2 &&
						m2->includesEdge(e1) &&
						m2->includesEdge(e2)) ;

		Edge e3 = CREATEEDGEOBJ (g2, 2, 6) ;
		m2->addEdge (e3) ;
		addTestResult (	m2->getCardinality() == 3 &&
						m2->includesEdge(e1) &&
						m2->includesEdge(e2) &&
						m2->includesEdge(e3)) ;

		m2->removeEdge (e2) ;
		addTestResult (	m2->getCardinality() == 2 &&
						m2->includesEdge(e1) &&
						m2->includesEdge(e3)) ;

		m2->removeEdge (e1) ;
		addTestResult (	m2->getCardinality() == 1 &&
						m2->includesEdge(e3)) ;

		m2->removeEdge (e3) ;
		addTestResult ( m2->getCardinality() == 0) ;
	}
}

void MatchingTest::testAugmentingPath ()
{
	{ // test Matching m1
		Globs = gl1 ;

		std::vector<Edge*> augpath1 ;
		augpath1.push_back (CREATEEDGEPTR (g1, 0, 1)) ;
		m1->augment (augpath1) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isExposed((VertexLabel) 2) &&
						m1->isExposed((VertexLabel) 3)) ;

		std::vector<Edge*> augpath2 ;
		augpath2.push_back (CREATEEDGEPTR (g1, 1, 3)) ;
		augpath2.push_back (CREATEEDGEPTR (g1, 0, 1)) ;
		augpath2.push_back (CREATEEDGEPTR (g1, 0, 2)) ;
		m1->augment (augpath2) ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1) &&
						m1->isMatched((VertexLabel) 2) &&
						m1->isMatched((VertexLabel) 3)) ;
	}

	{ // test Matching m2
		Globs = gl2 ;

		Edge* e12 = CREATEEDGEPTR (g2, 1, 2) ;
		Edge* e17 = CREATEEDGEPTR (g2, 1, 7) ;
		Edge* e23 = CREATEEDGEPTR (g2, 2, 3) ;
		Edge* e36 = CREATEEDGEPTR (g2, 3, 6) ;
		Edge* e47 = CREATEEDGEPTR (g2, 4, 7) ;
		Edge* e49 = CREATEEDGEPTR (g2, 4, 9) ;
		Edge* e56 = CREATEEDGEPTR (g2, 5, 6) ;

		std::vector<Edge*> augpath1 ;
		augpath1.push_back (e12) ;
		m2->augment (augpath1) ;
		addTestResult (	m2->getCardinality() == 1 &&
						m2->includesEdge(*e12)) ;

		std::vector<Edge*> augpath2 ;
		augpath2.push_back (e17) ;
		augpath2.push_back (e12) ;
		augpath2.push_back (e23) ;
		m2->augment (augpath2) ;
		addTestResult (	m2->getCardinality() == 2 &&
						m2->includesEdge(*e17) &&
						m2->includesEdge(*e23)) ;

		std::vector<Edge*> augpath3 ;
		augpath3.push_back (e47) ;
		augpath3.push_back (e17) ;
		augpath3.push_back (e12) ;
		augpath3.push_back (e23) ;
		augpath3.push_back (e36) ;
		m2->augment (augpath3) ;
		addTestResult (	m2->getCardinality() == 3 &&
						m2->includesEdge(*e47) &&
						m2->includesEdge(*e12) &&
						m2->includesEdge(*e36)) ;

		std::vector<Edge*> augpath4 ;
		augpath4.push_back (e49) ;
		augpath4.push_back (e47) ;
		augpath4.push_back (e17) ;
		augpath4.push_back (e12) ;
		augpath4.push_back (e23) ;
		augpath4.push_back (e36) ;
		augpath4.push_back (e56) ;
		m2->augment (augpath4) ;
		addTestResult (	m2->getCardinality() == 4 &&
						m2->includesEdge(*e49) &&
						m2->includesEdge(*e17) &&
						m2->includesEdge(*e23) &&
						m2->includesEdge(*e56)) ;
	}
}

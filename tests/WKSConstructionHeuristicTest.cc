/*
 * steghide 0.5.1 - a steganogrchy program
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
#include "BitString.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "Selector.h"

#include "DummyFile.h"
#include "WKSConstructionHeuristicTest.h"

#define CREATEEDGE(G,V1,V2) (new Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1))

WKSConstructionHeuristicTest::WKSConstructionHeuristicTest (TestSuite* s)
	: UnitTest ("WKSConstructionHeuristic", s)
{
	ADDTESTCATEGORY (WKSConstructionHeuristicTest, testAlgorithm) ;
}

void WKSConstructionHeuristicTest::setup ()
{
	UnitTest::setup() ;

	// a trivial case to start with
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (2) ;
		adjlist[0].push_back(1) ;
		DummyFile::createGraph (adjlist, &bs1, &f1, &s1) ;
		g1 = new Graph (f1, *bs1, *s1) ;
		m1 = new Matching (g1) ;
		ch1 = new WKSConstructionHeuristic (g1, m1, 100.0) ;
		gl1 = Globs ;
	}

	// first counterexample - will not find maximum matching
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (4) ;
		adjlist[0].push_back(1) ; adjlist[0].push_back(2) ; adjlist[0].push_back(3) ;
		adjlist[1].push_back(2) ; adjlist[1].push_back(3) ;
		DummyFile::createGraph (adjlist, &bs2, &f2, &s2) ;
		g2 = new Graph (f2, *bs2, *s2) ;
		m2 = new Matching (g2) ;
		ch2 = new WKSConstructionHeuristic (g2, m2, 100.0) ;
		gl2 = Globs ;
	}

	// will find maximum matching here because of Deg1-rule
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (4) ;
		adjlist[0].push_back(1) ; adjlist[0].push_back(2) ;
		adjlist[1].push_back(3) ;
		DummyFile::createGraph (adjlist, &bs3, &f3, &s3) ;
		g3 = new Graph (f3, *bs3, *s3) ;
		m3 = new Matching (g3) ;
		ch3 = new WKSConstructionHeuristic (g3, m3, 100.0) ;
		gl3 = Globs ;
	}
}

void WKSConstructionHeuristicTest::cleanup ()
{
	UnitTest::cleanup() ;

	Globs = gl1 ; delete ch1 ; delete m1 ; delete g1 ; delete s1 ; delete bs1 ; delete f1 ;
	Globs = gl2 ; delete ch2 ; delete m2 ; delete g2 ; delete s2 ; delete bs2 ; delete f2 ;
	Globs = gl3 ; delete ch3 ; delete m3 ; delete g3 ; delete s3 ; delete bs3 ; delete f3 ;
}

void WKSConstructionHeuristicTest::testAlgorithm ()
{
	{
		Globs = gl1 ;
		ch1->run() ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1)) ;
	}

	{
		Globs = gl2 ;
		ch2->run() ;
		addTestResult (	m2->isMatched((VertexLabel) 0) &&
						m2->isMatched((VertexLabel) 1) &&
						m2->isExposed((VertexLabel) 2) &&
						m2->isExposed((VertexLabel) 3)) ;
	}

	{
		Globs = gl3 ;
		ch3->run() ;
		addTestResult (	m3->isMatched((VertexLabel) 0) &&
						m3->isMatched((VertexLabel) 1) &&
						m3->isMatched((VertexLabel) 2) &&
						m3->isMatched((VertexLabel) 3)) ;
	}
}

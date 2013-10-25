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

#define private public
#define protected public
#include "BFSAPHeuristic.h"
#undef private
#undef protected
#include "BitString.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "Selector.h"

#include "DummyFile.h"
#include "BFSAPHeuristicTest.h"

#define CREATEEDGE(G,V1,V2) (new Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1))

BFSAPHeuristicTest::BFSAPHeuristicTest (TestSuite* s)
	: UnitTest ("BFSAPHeuristic", s)
{
	ADDTESTCATEGORY (BFSAPHeuristicTest, testAlgorithm) ;
}

void BFSAPHeuristicTest::setup ()
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
		aph1 = new BFSAPHeuristic (g1, m1) ;
		gl1 = Globs ;
	}

	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (4) ;
		adjlist[0].push_back(1) ;
		adjlist[1].push_back(2) ;
		adjlist[2].push_back(3) ;
		DummyFile::createGraph (adjlist, &bs2, &f2, &s2) ;
		g2 = new Graph (f2, *bs2, *s2) ;
		m2 = new Matching (g2) ;
		m2->addEdge (CREATEEDGE (g2, 1, 2)) ;
		aph2 = new BFSAPHeuristic (g2, m2) ;
		gl2 = Globs ;
	}

	// example: aph should be found in graph with blossom (walk through blossom in good direction)
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (4) ;
		adjlist[0].push_back(1) ; adjlist[0].push_back(2) ;
		adjlist[1].push_back(2) ;
		adjlist[2].push_back(3) ;
		DummyFile::createGraph (adjlist, &bs3, &f3, &s3) ;
		g3 = new Graph (f3, *bs3, *s3) ;
		m3 = new Matching (g3) ;
		m3->addEdge (CREATEEDGE (g3, 1, 2)) ;
		aph3 = new BFSAPHeuristic (g3, m3) ;
		gl3 = Globs ;
	}

	// counterexample: walk through blossom in bad direction
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (4) ;
		adjlist[0].push_back(1) ; adjlist[0].push_back(2) ;
		adjlist[1].push_back(2) ; adjlist[1].push_back(3) ;
		DummyFile::createGraph (adjlist, &bs4, &f4, &s4) ;
		g4 = new Graph (f4, *bs4, *s4) ;
		m4 = new Matching (g4) ;
		m4->addEdge (CREATEEDGE (g4, 1, 2)) ;
		aph4 = new BFSAPHeuristic (g4, m4) ;
		gl4 = Globs ;
	}
}

void BFSAPHeuristicTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete aph1 ; delete m1 ; delete g1 ; delete s1 ; delete bs1 ; delete f1 ;
	delete aph2 ; delete m2 ; delete g2 ; delete s2 ; delete bs2 ; delete f2 ;
	delete aph3 ; delete m3 ; delete g3 ; delete s3 ; delete bs3 ; delete f3 ;
	delete aph4 ; delete m4 ; delete g4 ; delete s4 ; delete bs4 ; delete f4 ;
}

void BFSAPHeuristicTest::testAlgorithm ()
{
	{
		Globs = gl1 ;
		aph1->run() ;
		addTestResult (	m1->isMatched((VertexLabel) 0) &&
						m1->isMatched((VertexLabel) 1)) ;
	}

	{
		Globs = gl2 ;
		Edge* e01 = CREATEEDGE (g2, 0, 1) ;
		Edge* e23 = CREATEEDGE (g2, 2, 3) ;
		aph2->run() ;
		addTestResult ( m2->getCardinality() == 2 &&
						m2->includesEdge(e01) &&
						m2->includesEdge(e23)) ;
	}

	{
		Globs = gl3 ;
		Edge* e01 = CREATEEDGE (g3, 0, 1) ;
		Edge* e23 = CREATEEDGE (g3, 2, 3) ;
		aph3->run() ;
		addTestResult ( m3->getCardinality() == 2 &&
						m3->includesEdge(e01) &&
						m3->includesEdge(e23)) ;
	}

	{
		Globs = gl4 ;
		Edge* e12 = CREATEEDGE (g4, 1, 2) ;
		aph4->run() ;
		addTestResult ( m4->getCardinality() == 1 &&
						m4->includesEdge(e12)) ;
	}
}

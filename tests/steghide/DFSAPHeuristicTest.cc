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
#include "DFSAPHeuristic.h"
#undef private
#undef protected
#include "BitString.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "Selector.h"

#include "DummyFile.h"
#include "DFSAPHeuristicTest.h"

#define CREATEEDGE(G,V1,V2) (new Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1))

DFSAPHeuristicTest::DFSAPHeuristicTest (TestSuite* s)
	: UnitTest ("DFSAPHeuristic", s)
{
	ADDTESTCATEGORY (DFSAPHeuristicTest, testAlgorithm) ;
}

void DFSAPHeuristicTest::setup ()
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
		aph1 = new DFSAPHeuristic (g1, m1, 100.0) ;
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
		aph2 = new DFSAPHeuristic (g2, m2, 100.0) ;
		gl2 = Globs ;
	}

	// this is the example from Moehring's and Mueller-Hannemann's paper (Figure 2)
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (8) ;
		adjlist[0].push_back(1) ;
		adjlist[1].push_back(2) ;
		adjlist[2].push_back(3) ; adjlist[2].push_back(6) ;
		adjlist[3].push_back(4) ;
		adjlist[4].push_back(5) ;
		adjlist[5].push_back(6) ; adjlist[5].push_back(7) ;
		DummyFile::createGraph (adjlist, &bs3, &f3, &s3) ;
		g3 = new Graph (f3, *bs3, *s3) ;
		m3 = new Matching (g3) ;
		m3->addEdge (CREATEEDGE (g3, 1, 2)) ;
		m3->addEdge (CREATEEDGE (g3, 3, 4)) ;
		m3->addEdge (CREATEEDGE (g3, 5, 6)) ;
		aph3 = new DFSAPHeuristic (g3, m3, 100.0) ;
		gl3 = Globs ;
	}

	// this is the counterexample from Moehring's and Mueller-Hannemann's paper (Figure 3)
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (8) ;
		adjlist[0].push_back(1) ; adjlist[0].push_back(6) ;
		adjlist[1].push_back(2) ; adjlist[1].push_back(7) ;
		adjlist[2].push_back(3) ; adjlist[2].push_back(6) ;
		adjlist[3].push_back(4) ;
		adjlist[4].push_back(5) ;
		adjlist[5].push_back(6) ;
		DummyFile::createGraph (adjlist, &bs4, &f4, &s4) ;
		g4 = new Graph (f4, *bs4, *s4) ;
		m4 = new Matching (g4) ;
		m4->addEdge (CREATEEDGE (g4, 1, 2)) ;
		m4->addEdge (CREATEEDGE (g4, 3, 4)) ;
		m4->addEdge (CREATEEDGE (g4, 5, 6)) ;
		aph4 = new DFSAPHeuristic (g4, m4, 100.0) ;
		gl4 = Globs ;
	}

	// another counterexample where the existing augmenting path will not be found no matter which start vertex is chosen
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist (14) ;
		adjlist[0].push_back(2) ; adjlist[0].push_back(4) ;
		adjlist[1].push_back(3) ; adjlist[1].push_back(5) ;
		adjlist[2].push_back(3) ; adjlist[2].push_back(12) ;
		adjlist[3].push_back(13) ;
		adjlist[4].push_back(6) ; adjlist[4].push_back(12) ;
		adjlist[5].push_back(7) ; adjlist[5].push_back(13) ;
		adjlist[6].push_back(8) ;
		adjlist[7].push_back(9) ;
		adjlist[8].push_back(10) ;
		adjlist[9].push_back(11) ;
		adjlist[10].push_back(12) ;
		adjlist[11].push_back(13) ;
		DummyFile::createGraph (adjlist, &bs5, &f5, &s5) ;
		g5 = new Graph (f5, *bs5, *s5) ;
		m5 = new Matching (g5) ;
		m5->addEdge (CREATEEDGE (g5, 2, 12)) ;
		m5->addEdge (CREATEEDGE (g5, 3, 13)) ;
		m5->addEdge (CREATEEDGE (g5, 4, 6)) ;
		m5->addEdge (CREATEEDGE (g5, 5, 7)) ;
		m5->addEdge (CREATEEDGE (g5, 8, 10)) ;
		m5->addEdge (CREATEEDGE (g5, 9, 11)) ;
		aph5 = new DFSAPHeuristic (g5, m5, 100.0) ;
		gl5 = Globs ;
	}
}

void DFSAPHeuristicTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete aph1 ; delete m1 ; delete g1 ; delete s1 ; delete bs1 ; delete f1 ;
	delete aph2 ; delete m2 ; delete g2 ; delete s2 ; delete bs2 ; delete f2 ;
	delete aph3 ; delete m3 ; delete g3 ; delete s3 ; delete bs3 ; delete f3 ;
	delete aph4 ; delete m4 ; delete g4 ; delete s4 ; delete bs4 ; delete f4 ;
	delete aph5 ; delete m5 ; delete g5 ; delete s5 ; delete bs5 ; delete f5 ;
}

void DFSAPHeuristicTest::testAlgorithm ()
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
		Edge* e26 = CREATEEDGE (g3, 2, 6) ;
		Edge* e34 = CREATEEDGE (g3, 3, 4) ;
		Edge* e57 = CREATEEDGE (g3, 5, 7) ;
		aph3->run() ;
		addTestResult ( m3->getCardinality() == 4 &&
						m3->includesEdge(e01) &&
						m3->includesEdge(e26) &&
						m3->includesEdge(e34) &&
						m3->includesEdge(e57)) ;
	}

	{
		Globs = gl4 ;
		const Edge** path = new const Edge*[g4->getNumVertices()] ;
		unsigned long len = aph4->searchAugmentingPath (g4->getVertex(0), path) ;
		addTestResult (len == 0) ;
		delete[] path ;
	}

	{
		Globs = gl5 ;
		Edge* e2_12 = CREATEEDGE (g5, 2, 12) ;
		Edge* e3_13 = CREATEEDGE (g5, 3, 13) ;
		Edge* e4_6 = CREATEEDGE (g5, 4, 6) ;
		Edge* e5_7 = CREATEEDGE (g5, 5, 7) ;
		Edge* e8_10 = CREATEEDGE (g5, 8, 10) ;
		Edge* e9_11 = CREATEEDGE (g5, 9, 11) ;
		aph5->run() ;
		addTestResult ( m5->getCardinality() == 6 &&
						m5->includesEdge(e2_12) &&
						m5->includesEdge(e3_13) &&
						m5->includesEdge(e4_6) &&
						m5->includesEdge(e5_7) &&
						m5->includesEdge(e8_10) &&
						m5->includesEdge(e9_11)) ;
	}
}

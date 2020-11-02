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
#include "common.h"

#include "DummyFile.h"
#include "EdgeIteratorTest.h"
#include "TestCategoryCaller.h"
#include "utcommon.h"

#define CREATEEDGE(G,V1,V2) ((V1 < V2) ? \
		(new Edge ((G)->getVertex(V1), 0, (G)->getVertex(V2), 1)) : \
		(new Edge ((G)->getVertex(V2), 0, (G)->getVertex(V1), 1)))

EdgeIteratorTest::EdgeIteratorTest (TestSuite* s)
	: UnitTest ("EdgeIterator", s)
{
	ADDTESTCATEGORY (EdgeIteratorTest, testReferenceIteration) ;
	ADDTESTCATEGORY (EdgeIteratorTest, testIterationLength) ;
}

void EdgeIteratorTest::setup ()
{
	// reference tests
	{
		Globs.reset() ;
		std::vector<std::list<UWORD16> > adjlist1 (10) ;
		adjlist1[0].push_back(1) ; adjlist1[0].push_back(4) ;
		adjlist1[1].push_back(3) ; adjlist1[1].push_back(7) ;
		adjlist1[2].push_back(3) ; adjlist1[2].push_back(5) ; adjlist1[2].push_back(6) ;
		adjlist1[3].push_back(6) ;
		adjlist1[4].push_back(6) ; adjlist1[4].push_back(7) ; adjlist1[4].push_back(9) ;
		adjlist1[5].push_back(6) ;
		adjlist1[7].push_back(8) ;
		DummyFile::createGraph (adjlist1, &bs1, &f1, &s1) ;
		g1 = new Graph (f1, *bs1, *s1) ;
		gl1 = Globs ;
	}

	// real files
	{
		Globs.reset() ;
		f10 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
		bs10 = new BitString (std::string ("secret message")) ;
		s10 = new Selector (bs10->getLength() * f10->getSamplesPerVertex(), std::string ("secret passphrase")) ;
		g10 = new Graph (f10, *bs10, *s10) ;
		gl10 = Globs ;

		Globs.reset() ;
		f11 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.wav") ;
		bs11 = new BitString (std::string ("16 bits per sample")) ;
		s11 = new Selector (bs11->getLength() * f11->getSamplesPerVertex(), std::string ("a passphrase")) ;
		g11 = new Graph (f11, *bs11, *s11) ;
		gl11 = Globs ;

		Globs.reset() ;
		f12 = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;
		bs12 = new BitString (std::string ("mu law sample data in an au file")) ;
		s12 = new Selector (bs12->getLength() * f12->getSamplesPerVertex(), std::string ("another passphrase")) ;
		g12 = new Graph (f12, *bs12, *s12) ;
		gl12 = Globs ;
	}
}

void EdgeIteratorTest::cleanup ()
{
}

void EdgeIteratorTest::testReferenceIteration ()
{
	{
		Globs = gl1 ;

		std::vector<Edge*> edges ;
		edges.push_back (CREATEEDGE (g1, 0, 1)) ;
		edges.push_back (CREATEEDGE (g1, 0, 4)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(0), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 1, 0)) ;
		edges.push_back (CREATEEDGE (g1, 1, 3)) ;
		edges.push_back (CREATEEDGE (g1, 1, 7)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(1), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 2, 3)) ;
		edges.push_back (CREATEEDGE (g1, 2, 5)) ;
		edges.push_back (CREATEEDGE (g1, 2, 6)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(2), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 3, 2)) ;
		edges.push_back (CREATEEDGE (g1, 3, 1)) ;
		edges.push_back (CREATEEDGE (g1, 3, 6)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(3), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 4, 6)) ;
		edges.push_back (CREATEEDGE (g1, 4, 7)) ;
		edges.push_back (CREATEEDGE (g1, 4, 0)) ;
		edges.push_back (CREATEEDGE (g1, 4, 9)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(4), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 5, 6)) ;
		edges.push_back (CREATEEDGE (g1, 5, 2)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(5), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 6, 5)) ;
		edges.push_back (CREATEEDGE (g1, 6, 4)) ;
		edges.push_back (CREATEEDGE (g1, 6, 3)) ;
		edges.push_back (CREATEEDGE (g1, 6, 2)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(6), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 7, 8)) ;
		edges.push_back (CREATEEDGE (g1, 7, 4)) ;
		edges.push_back (CREATEEDGE (g1, 7, 1)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(7), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 8, 7)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(8), EdgeIterator::SAMPLEOCCURENCE, edges)) ;

		edges.clear() ;
		edges.push_back (CREATEEDGE (g1, 9, 4)) ;
		addTestResult (genericTestVertexIteration (g1->getVertex(9), EdgeIterator::SAMPLEOCCURENCE, edges)) ;
	}
}

void EdgeIteratorTest::testIterationLength ()
{
	Globs = gl10 ; addTestResult (genericTestIterationLength (g10)) ;
	Globs = gl11 ; addTestResult (genericTestIterationLength (g11)) ;
}

#if 0
bool EdgeIteratorTest::genericTestGraphIteration (Graph* g, EdgeIterator::ITERATIONMODE m)
{
	for (VertexLabel vlbl = 0 ; vlbl < g->Vertices.size() ; vlbl++) {
	}
}
#endif

bool EdgeIteratorTest::genericTestVertexIteration (Vertex* srcvertex, EdgeIterator::ITERATIONMODE m, const std::vector<Edge*>& edges)
{
	EdgeIterator edgeit (srcvertex, m) ;
	std::vector<Edge*>::const_iterator refit = edges.begin() ;

	bool ok = true ;
	while (!edgeit.isFinished() && refit != edges.end()) {
		if (**refit != **edgeit) {
			if (ArgVerbose) {
				if (ok) {
					std::cerr << std::endl << "---- FAILED: genericTestVertexIteration ----" << std::endl ;
				}
				std::cerr << "expected edge:" << std::endl ;
				(*refit)->print(1) ;
				std::cerr << "found edge:" << std::endl ;
				Edge* e = new Edge(**edgeit) ;
				e->print(1) ;
				delete e ;
			}
			ok = false ;
		}

		++refit ;
		++edgeit ;
	}

	bool lenok = (edgeit.isFinished() && (refit == edges.end())) ;

	if (!lenok && ArgVerbose) {
		if (ok) {
			std::cerr << std::endl << "---- FAILED: genericTestVertexIteration ----" << std::endl ;
		}
		std::cerr << "length problem:" << std::endl ;
		std::cerr << "edgeit is" << ((edgeit.isFinished()) ? "" : " not") << " finished" << std::endl ;
		std::cerr << "refit is" << ((refit == edges.end()) ? "" : " not") << " finished" << std::endl ;

		std::cerr << "rest of reference iteration:" << std::endl ;
		for ( ; refit != edges.end() ; refit++) {
			(*refit)->print(1) ;
		}
		if (ok) {
			std::cerr << "-------------------------------------" << std::endl ;
		}
	}

	if (!ok && ArgVerbose) {
		std::cerr << "-------------------------------------" << std::endl ;
	}

	return (lenok && ok) ;
}

bool EdgeIteratorTest::genericTestIterationLength (Graph* g)
{
	bool ok = true ;
	for (VertexLabel l = 0 ; l < g->getNumVertices() ; l++) {
		Vertex* srcvertex = g->getVertex(l) ;

		EdgeIterator edgeit (srcvertex) ;
		unsigned long n = 0 ;
		while (!edgeit.isFinished()) {
			++edgeit ;
			n++ ;
		}

		if (n != srcvertex->getDegree() && ArgVerbose) {
			if (ok) {
				std::cerr << std::endl << "---- FAILED: genericTestIterationLength ----" << std::endl ;
			}
			std::cerr << "for vertex " << l << ": getDegree(): " << srcvertex->getDegree() << ", edgeit goes through: " << n << std::endl ;
		}
		ok = (n == srcvertex->getDegree()) && ok ;

	}

	if (!ok && ArgVerbose) {
		std::cerr << "-------------------------------------" << std::endl ;
	}

	return ok ;
}

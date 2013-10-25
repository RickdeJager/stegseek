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

#ifndef SH_EDGEITERATORTEST_H
#define SH_EDGEITERATORTEST_H

#include "BitString.h"
#include "CvrStgFile.h"
#define private public
#define protected public
#include "Graph.h"
#undef protected
#undef private
#include "EdgeIterator.h"
#include "Selector.h"

#include "UnitTest.h"
#include "TestSuite.h"

class EdgeIteratorTest : public UnitTest {
	public:
	EdgeIteratorTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testReferenceIteration (void) ;
	void testIterationLength (void) ;

	private:
	BitString *bs1, *bs2 ;
	CvrStgFile *f1, *f2 ;
	Selector *s1, *s2 ;
	Graph *g1, *g2 ;
	Globals gl1, gl2 ;

	BitString *bs10, *bs11, *bs12 ;
	CvrStgFile *f10, *f11, *f12 ;
	Selector *s10, *s11, *s12 ;
	Graph *g10, *g11, *g12 ;
	Globals gl10, gl11, gl12 ;

	bool genericTestGraphIteration (Graph* g, EdgeIterator::ITERATIONMODE m) ;

	/**
	 * check if an edge iterator for srcvertex with m iterates exactly through edges
	 **/
	bool genericTestVertexIteration (Vertex* srcvertex, EdgeIterator::ITERATIONMODE m, const std::vector<Edge*>& edges) ;

	/**
	 * for all vertices in the graph test if getDegree() returns exactly the number of edges EdgeIterator iterates through
	 **/
	bool genericTestIterationLength (Graph* g) ;
} ;

#endif // ndef SH_EDGEITERATORTEST_H

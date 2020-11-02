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

#ifndef SH_GRAPHTEST_H
#define SH_GRAPHTEST_H

#include "UnitTest.h"

class TestSuite ;
class CvrStgFile ;
class BitString ;
class Selector ;
class Graph ;

class GraphTest : public UnitTest {
	public:
	GraphTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testVertices (void) ;
	void testSampleValues (void) ;
	void testSampleOccurences (void) ;
	void testSampleValueAdjacencyLists (void) ;
	
	private:
	CvrStgFile *f1, *f2, *f3, *f4, *f5, *f6, *f7, *f8, *f9, *f10, *f11, *f12, *f13, *f14, *f15, *f_f ;
	BitString *bs1, *bs2, *bs3, *bs4, *bs5, *bs6, *bs7, *bs8, *bs9, *bs10, *bs11, *bs12, *bs13, *bs14, *bs15, *bs_f ;
	Selector *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9, *s10, *s11, *s12, *s13, *s14, *s15, *s_f ;
	Graph *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *g9, *g10, *g11, *g12, *g13, *g14, *g15, *g_f ;
	Globals gl1, gl2, gl3, gl4, gl5, gl6, gl7, gl8, gl9, gl10, gl11, gl12, gl13, gl14, gl15, gl_f ;
} ;

#endif // ndef SH_GRAPHTEST_H

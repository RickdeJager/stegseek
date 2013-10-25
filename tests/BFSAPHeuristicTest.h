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

#ifndef SH_BFSAPHEURISTICTEST_H
#define SH_BFSAPHEURISTICTEST_H

class BFSAPHeuristic ;
class BitString ;
class CvrStgFile ;
class Matching ;
class Selector ;

#include "UnitTest.h"
#include "TestSuite.h"

class BFSAPHeuristicTest : public UnitTest {
	public:
	BFSAPHeuristicTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testAlgorithm (void) ;

	private:
	BitString *bs1, *bs2, *bs3, *bs4, *bs5 ;
	CvrStgFile *f1, *f2, *f3, *f4, *f5 ;
	Selector *s1, *s2, *s3, *s4, *s5 ;
	Graph *g1, *g2, *g3, *g4, *g5 ;
	Matching *m1, *m2, *m3, *m4, *m5 ;
	BFSAPHeuristic *aph1, *aph2, *aph3, *aph4, *aph5 ;
	Globals gl1, gl2, gl3, gl4, gl5 ;
} ;

#endif // ndef SH_BFSAPHEURISTICTEST_H

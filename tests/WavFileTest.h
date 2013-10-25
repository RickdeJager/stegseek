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

#ifndef SH_WAVFILETEST_H
#define SH_WAVFILETEST_H

#include "CvrStgFileTest.h"

#include "Selector.h"

class WavFileTest : public CvrStgFileTest {
	public:
	WavFileTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testReadWrite (void) ;
	void testReadEmbedExtract (void) ;
	void testReadEmbedWriteReadExtract (void) ;
	void testPosition (void) ;
	void testReadExtractCompare (void) ;
	void testSVALCalculation (void) ;
	void testEmbeddedValue (void) ;

	private:
	BitString *bs1, *bs2 ;
	CvrStgFile *f1, *f2 ;
	Graph *g1, *g2 ;
	Selector *s1, *s2 ;
	Globals gl1, gl2 ;
} ;

#endif // ndef SH_BMPWINFILETEST_H

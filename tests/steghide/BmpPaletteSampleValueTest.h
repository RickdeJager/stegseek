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

#ifndef SH_BMPPALETTESAMPLEVALUETEST_H
#define SH_BMPPALETTESAMPLEVALUETEST_H

#include "SampleValueTest.h"

class CvrStgFile ;

class BmpPaletteSampleValueTest : public SampleValueTest {
	public:
	BmpPaletteSampleValueTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testDistance (void) ;
	void testIsNeighbour (void) ;

	private:
	CvrStgFile *f1, *f2 ;
	Globals gl1, gl2 ;
	SampleValue *sv1_0, *sv1_3, *sv1_7, *sv1_15 ;
	SampleValue *sv2_3_49_96_5, *sv2_10_11_99_203, *sv2_48_18_106_194, *sv2_68_73_104_15, *sv2_171_61_97_25 ;
} ;

#endif // ndef SH_BMPPALETTESAMPLEVALUETEST_H

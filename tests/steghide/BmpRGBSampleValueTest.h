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

#ifndef SH_BMPRGBSAMPLEVALUETEST_H
#define SH_BMPRGBSAMPLEVALUETEST_H

#include "SampleValueTest.h"

class CvrStgFile ;

class BmpRGBSampleValueTest : public SampleValueTest {
	public:
	BmpRGBSampleValueTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testDistance (void) ;
	void testIsNeighbour (void) ;

	private:
	CvrStgFile *f_win, *f_os2 ;
	SampleValue *sv_0_0_0, *sv_1_1_1, *sv_0_3_4, *sv_10_10_10, *sv_12_13_14, *sv_128_128_128, *sv_210_0_120, *sv_255_255_255 ;
	Globals gl_win, gl_os2 ;
} ;

#endif // ndef SH_BMPRGBSAMPLEVALUETEST_H

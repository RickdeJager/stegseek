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

#ifndef SH_WAVPCMSAMPLEVALUETEST_H
#define SH_WAVPCMSAMPLEVALUETEST_H

#include "SampleValueTest.h"

class CvrStgFile ;

class WavPCMSampleValueTest : public SampleValueTest {
	public:
	WavPCMSampleValueTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testDistance (void) ;
	void testIsNeighbour (void) ;

	private:
	CvrStgFile *f_WavPCM8, *f_WavPCM16 ;
	SampleValue *sv_WavPCM8_0, *sv_WavPCM8_1, *sv_WavPCM8_45, *sv_WavPCM16_m32768, *sv_WavPCM16_32767, *sv_WavPCM16_0, *sv_WavPCM16_15 ;
	Globals gl_WavPCM8, gl_WavPCM16 ;
} ;

#endif // ndef SH_WAVPCMSAMPLEVALUETEST_H

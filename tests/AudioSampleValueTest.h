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

#ifndef SH_AUDIOSAMPLEVALUETEST_H
#define SH_AUDIOSAMPLEVALUETEST_H

#include "SampleValueTest.h"

class CvrStgFile ;

class AudioSampleValueTest : public SampleValueTest {
	public:
	AudioSampleValueTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testDistance (void) ;
	void testIsNeighbour (void) ;

	private:
	CvrStgFile *f_AuMuLaw, *f_AuPCM16 ;
	SampleValue *sv_AuMuLaw_0, *sv_AuMuLaw_1, *sv_AuMuLaw_45, *sv_AuPCM16_m32768, *sv_AuPCM16_32767, *sv_AuPCM16_0, *sv_AuPCM16_15 ;
	Globals gl_AuMuLaw, gl_AuPCM16 ;
} ;

#endif // ndef SH_AUDIOSAMPLEVALUETEST_H

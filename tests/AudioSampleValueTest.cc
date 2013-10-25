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

#include "AudioSampleValue.h"
#include "AuSampleValues.h"
#include "CvrStgFile.h"

#include "AudioSampleValueTest.h"
#include "utcommon.h"

AudioSampleValueTest::AudioSampleValueTest (TestSuite* s)
	: SampleValueTest ("AudioSampleValue", s)
{
	ADDTESTCATEGORY (AudioSampleValueTest, testDistance) ;
	ADDTESTCATEGORY (AudioSampleValueTest, testIsNeighbour) ;
}

void AudioSampleValueTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f_AuMuLaw = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;
	sv_AuMuLaw_0 = new AuMuLawSampleValue (0) ;
	sv_AuMuLaw_1 = new AuMuLawSampleValue (1) ;
	sv_AuMuLaw_45 = new AuMuLawSampleValue (45) ;
	gl_AuMuLaw = Globs ;

	Globs.reset() ;
	f_AuPCM16 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.au") ;
	sv_AuPCM16_m32768 = new AuPCM16SampleValue (-32768) ;
	sv_AuPCM16_32767 = new AuPCM16SampleValue (32767) ;
	sv_AuPCM16_0 = new AuPCM16SampleValue (0) ;
	sv_AuPCM16_15 = new AuPCM16SampleValue (15) ;
	gl_AuPCM16 = Globs ;
}

void AudioSampleValueTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete f_AuMuLaw ; delete f_AuPCM16 ;
	delete sv_AuMuLaw_0 ; delete sv_AuMuLaw_1 ; delete sv_AuMuLaw_45 ;
	delete sv_AuPCM16_m32768 ; delete sv_AuPCM16_32767 ; delete sv_AuPCM16_0 ; delete sv_AuPCM16_15 ;
}

void AudioSampleValueTest::testDistance ()
{
	Globs = gl_AuMuLaw ;
	addTestResult (genericTestDistance (sv_AuMuLaw_0, sv_AuMuLaw_1, 1)) ;
	addTestResult (genericTestDistance (sv_AuMuLaw_1, sv_AuMuLaw_45, 44)) ;

	Globs = gl_AuPCM16 ;
	addTestResult (genericTestDistance (sv_AuPCM16_0, sv_AuPCM16_15, 15)) ;
	addTestResult (genericTestDistance (sv_AuPCM16_15, sv_AuPCM16_32767, 32752)) ;
	addTestResult (genericTestDistance (sv_AuPCM16_m32768, sv_AuPCM16_32767, 65535)) ;
}

void AudioSampleValueTest::testIsNeighbour ()
{
	Globs = gl_AuMuLaw ;
	addTestResult (genericTestIsNeighbour (sv_AuMuLaw_0, sv_AuMuLaw_1, true)) ;
	addTestResult (genericTestIsNeighbour (sv_AuMuLaw_0, sv_AuMuLaw_45, false)) ;

	Globs = gl_AuPCM16 ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_0, sv_AuPCM16_15, true)) ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_m32768, sv_AuPCM16_0, false)) ;
	addTestResult (genericTestIsNeighbour (sv_AuPCM16_m32768, sv_AuPCM16_32767, false)) ;
}

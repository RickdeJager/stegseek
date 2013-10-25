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

#include "JpegSampleValueTest.h"
#include "utcommon.h"

#include "JpegSampleValue.h"
#include "CvrStgFile.h"

JpegSampleValueTest::JpegSampleValueTest (TestSuite* s)
	: SampleValueTest ("JpegSampleValue", s)
{
	ADDTESTCATEGORY (JpegSampleValueTest, testDistance) ;
	ADDTESTCATEGORY (JpegSampleValueTest, testIsNeighbour) ;
}

void JpegSampleValueTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "std.jpg") ;
	sv_m1 = new JpegSampleValue (-1) ;
	sv_0 = new JpegSampleValue (0) ;
	sv_1 = new JpegSampleValue (1) ;
	gl1 = Globs ;
}

void JpegSampleValueTest::cleanup ()
{
	UnitTest::cleanup() ;
	delete f1 ;
	delete sv_m1 ; delete sv_0 ; delete sv_1 ;
}

void JpegSampleValueTest::testDistance ()
{
	Globs = gl1 ;
	addTestResult (genericTestDistance (sv_0, sv_1, 1)) ;
	addTestResult (genericTestDistance (sv_m1, sv_1, 2)) ;
}

void JpegSampleValueTest::testIsNeighbour ()
{
	Globs = gl1 ;
	addTestResult (genericTestIsNeighbour (sv_m1, sv_0, true)) ;
	addTestResult (genericTestIsNeighbour (sv_1, sv_m1, false)) ;
}

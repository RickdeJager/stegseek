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

#include <iostream>

#include "Graph.h"
#include "WavPCMSampleValue.h"

#include "WavFileTest.h"
#include "utcommon.h"

WavFileTest::WavFileTest (TestSuite* s)
	: CvrStgFileTest("WavFile", s)
{
	ADDTESTCATEGORY (WavFileTest, testReadWrite) ;
	ADDTESTCATEGORY (WavFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (WavFileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (WavFileTest, testPosition) ;
	ADDTESTCATEGORY (WavFileTest, testSVALCalculation) ;
	ADDTESTCATEGORY (WavFileTest, testEmbeddedValue) ;
}

void WavFileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "pcm8_std.wav") ;
	bs1 = new BitString (std::string ("this is a test string for the WavFile")) ;
	s1 = new Selector (bs1->getLength() * f1->getSamplesPerVertex(), std::string ("a passphrase")) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.wav") ;
	bs2 = new BitString (std::string ("this is another test - this time a little longer, but also for the WavFile unit test")) ;
	s2 = new Selector (bs2->getLength() * f2->getSamplesPerVertex(), std::string ("another passphrase")) ;
	g2 = new Graph (f2, *bs2, *s2) ;
	gl2 = Globs ;
}

void WavFileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete g1 ; delete g2 ;
	delete s1 ; delete s2 ;
	delete bs1 ; delete bs2 ;
	delete f1 ; delete f2 ;
}

void WavFileTest::testReadWrite()
{
	Globs = gl1 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm8_std.wav")) ;
	Globs = gl2 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm16_std.wav")) ;
}

void WavFileTest::testReadEmbedExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm8_std.wav", *bs1)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm16_std.wav", *bs2)) ;
}

void WavFileTest::testReadEmbedWriteReadExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm8_std.wav", *bs2)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm16_std.wav", *bs1)) ;
}

void WavFileTest::testPosition()
{
	Globs = gl1 ;
	addTestResult (genericTestPosition (f1, 0, new WavPCMSampleValue (122))) ;
	addTestResult (genericTestPosition (f1, 22, new WavPCMSampleValue (127))) ;

	Globs = gl2 ;
	addTestResult (genericTestPosition (f2, 0, new WavPCMSampleValue (-425))) ;
	addTestResult (genericTestPosition (f2, 15, new WavPCMSampleValue (136))) ;
}

void WavFileTest::testReadExtractCompare ()
{
	// TODO
}

void WavFileTest::testSVALCalculation ()
{
	Globs = gl1 ; addTestResult (genericTestSVALCalculation (f1, g1)) ;
	Globs = gl2 ; addTestResult (genericTestSVALCalculation (f2, g2)) ;
}

void WavFileTest::testEmbeddedValue ()
{
	Globs = gl1 ; addTestResult (genericTestEmbeddedValue (f1)) ;
	Globs = gl2 ; addTestResult (genericTestEmbeddedValue (f2)) ;
}

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

#include "utcommon.h"
#include "BmpWinFileTest.h"

#include "BmpPaletteSampleValue.h"
#include "BmpRGBSampleValue.h"

BmpWinFileTest::BmpWinFileTest (TestSuite* s)
	: CvrStgFileTest("BmpWinFile", s)
{
	ADDTESTCATEGORY (BmpWinFileTest, testReadWrite) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (BmpWinFileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (BmpWinFileTest, testPosition) ;
	ADDTESTCATEGORY (BmpWinFileTest, testEmbeddedValue) ;
}

void BmpWinFileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "win3x1_std.bmp") ;
	bs1 = new BitString (std::string ("this is a test")) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "win3x4_std.bmp") ;
	bs2 = new BitString (std::string ("this is another test - this time a little longer")) ;
	gl2 = Globs ;

	Globs.reset() ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
	bs3 = new BitString (std::string ("yet another test which is even longer than the previous test bitstring")) ;
	gl3 = Globs ;

	Globs.reset() ;
	f4 = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs4 = new BitString (std::string ("the last test")) ;
	gl4 = Globs ;
}

void BmpWinFileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete bs1 ; delete bs2 ; delete bs3 ; delete bs4 ;
	delete f1 ; delete f2 ; delete f3 ; delete f4 ;
}

void BmpWinFileTest::testReadWrite()
{
	Globs = gl1 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "win3x1_std.bmp")) ;
	Globs = gl2 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "win3x4_std.bmp")) ;
	Globs = gl3 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "win3x8_std.bmp")) ;
	Globs = gl4 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "win3x24_std.bmp")) ;
}

void BmpWinFileTest::testReadEmbedExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "win3x1_std.bmp", *bs1)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "win3x4_std.bmp", *bs2)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "win3x8_std.bmp", *bs3)) ;
	Globs = gl4 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "win3x24_std.bmp", *bs4)) ;
}

void BmpWinFileTest::testReadEmbedWriteReadExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "win3x1_std.bmp", *bs2)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "win3x4_std.bmp", *bs3)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "win3x8_std.bmp", *bs4)) ;
	Globs = gl4 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "win3x24_std.bmp", *bs1)) ;
}

void BmpWinFileTest::testPosition()
{
	Globs = gl1 ;
	addTestResult (genericTestPosition (f1, 0, new BmpPaletteSampleValue (0))) ;
	addTestResult (genericTestPosition (f1, 1, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 143, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 144, new BmpPaletteSampleValue (0))) ;
	addTestResult (genericTestPosition (f1, 2303, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 2302, new BmpPaletteSampleValue (0))) ;

	Globs = gl2 ;
	addTestResult (genericTestPosition (f2, 0, new BmpPaletteSampleValue (9))) ;
	addTestResult (genericTestPosition (f2, 20, new BmpPaletteSampleValue (4))) ;
	addTestResult (genericTestPosition (f2, 47, new BmpPaletteSampleValue (13))) ;
	addTestResult (genericTestPosition (f2, 48, new BmpPaletteSampleValue (9))) ;
	addTestResult (genericTestPosition (f2, 2303, new BmpPaletteSampleValue (3))) ;

	Globs = gl3 ;
	addTestResult (genericTestPosition (f3, 0, new BmpPaletteSampleValue (202))) ;
	addTestResult (genericTestPosition (f3, 4, new BmpPaletteSampleValue (38))) ;
	addTestResult (genericTestPosition (f3, 47, new BmpPaletteSampleValue (81))) ;
	addTestResult (genericTestPosition (f3, 48, new BmpPaletteSampleValue (66))) ;
	addTestResult (genericTestPosition (f3, 2302, new BmpPaletteSampleValue (105))) ;

	Globs = gl4 ;
	addTestResult (genericTestPosition (f4, 0, new BmpRGBSampleValue (121, 110, 119))) ;
	addTestResult (genericTestPosition (f4, 1, new BmpRGBSampleValue (113, 109, 130))) ;
	addTestResult (genericTestPosition (f4, 12, new BmpRGBSampleValue (92, 113, 115))) ;
	addTestResult (genericTestPosition (f4, 2303, new BmpRGBSampleValue (75, 107, 158))) ;
}

void BmpWinFileTest::testReadExtractCompare()
{
	// TODO
}

void BmpWinFileTest::testEmbeddedValue ()
{
	Globs = gl1 ; addTestResult (genericTestEmbeddedValue (f1)) ;
	Globs = gl2 ; addTestResult (genericTestEmbeddedValue (f2)) ;
	Globs = gl3 ; addTestResult (genericTestEmbeddedValue (f3)) ;
	Globs = gl4 ; addTestResult (genericTestEmbeddedValue (f4)) ;
}

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

#include "BmpPaletteSampleValue.h"
#include "BmpRGBSampleValue.h"

#include "utcommon.h"
#include "BmpOS2FileTest.h"

BmpOS2FileTest::BmpOS2FileTest (TestSuite* s)
	: CvrStgFileTest("BmpOS2File", s)
{
	ADDTESTCATEGORY (BmpOS2FileTest, testReadWrite) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testPosition) ;
	ADDTESTCATEGORY (BmpOS2FileTest, testEmbeddedValue) ;
}

void BmpOS2FileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	bs1 = new BitString (std::string ("a test")) ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "os21x1_std.bmp") ;
	gl1 = Globs ;

	Globs.reset() ;
	bs2 = new BitString (std::string ("another test - this time a little longer")) ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "os21x4_std.bmp") ;
	gl2 = Globs ;

	Globs.reset() ;
	bs3 = new BitString (std::string ("yet another test")) ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "os21x8_std.bmp") ;
	gl3 = Globs ;

	Globs.reset() ;
	bs4 = new BitString (std::string ("last test")) ;
	f4 = CvrStgFile::readFile (std::string(DATADIR) + "os21x24_std.bmp") ;
	gl4 = Globs ;
}

void BmpOS2FileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete bs1 ; delete bs2 ; delete bs3 ; delete bs4 ;
	delete f1 ; delete f2 ; delete f3 ; delete f4 ;
}

void BmpOS2FileTest::testReadWrite (void)
{
	Globs = gl1 ; addTestResult(genericTestReadWrite(std::string(DATADIR) + "os21x1_std.bmp")) ;
	Globs = gl2 ; addTestResult(genericTestReadWrite(std::string(DATADIR) + "os21x4_std.bmp")) ;
	Globs = gl3 ; addTestResult(genericTestReadWrite(std::string(DATADIR) + "os21x8_std.bmp")) ;
	Globs = gl4 ; addTestResult(genericTestReadWrite(std::string(DATADIR) + "os21x24_std.bmp")) ;
}

void BmpOS2FileTest::testReadEmbedExtract (void)
{
	Globs = gl1 ; addTestResult(genericTestReadEmbedExtract(std::string(DATADIR) + "os21x1_std.bmp", *bs1)) ;
	Globs = gl2 ; addTestResult(genericTestReadEmbedExtract(std::string(DATADIR) + "os21x4_std.bmp", *bs2)) ;
	Globs = gl3 ; addTestResult(genericTestReadEmbedExtract(std::string(DATADIR) + "os21x8_std.bmp", *bs3)) ;
	Globs = gl4 ; addTestResult(genericTestReadEmbedExtract(std::string(DATADIR) + "os21x24_std.bmp", *bs4)) ;
}

void BmpOS2FileTest::testReadEmbedWriteReadExtract (void)
{
	Globs = gl1 ; addTestResult(genericTestReadEmbedWriteReadExtract(std::string(DATADIR) + "os21x1_std.bmp", *bs2)) ;
	Globs = gl2 ; addTestResult(genericTestReadEmbedWriteReadExtract(std::string(DATADIR) + "os21x4_std.bmp", *bs3)) ;
	Globs = gl3 ; addTestResult(genericTestReadEmbedWriteReadExtract(std::string(DATADIR) + "os21x8_std.bmp", *bs4)) ;
	Globs = gl4 ; addTestResult(genericTestReadEmbedWriteReadExtract(std::string(DATADIR) + "os21x24_std.bmp", *bs1)) ;
}

void BmpOS2FileTest::testPosition()
{
	Globs = gl1 ;
	addTestResult (genericTestPosition (f1, 0, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 1, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 2, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f1, 50, new BmpPaletteSampleValue (0))) ;
	addTestResult (genericTestPosition (f1, 2300, new BmpPaletteSampleValue (0))) ;
	addTestResult (genericTestPosition (f1, 2303, new BmpPaletteSampleValue (1))) ;

	Globs = gl2 ;
	addTestResult (genericTestPosition (f2, 0, new BmpPaletteSampleValue (7))) ;
	addTestResult (genericTestPosition (f2, 10, new BmpPaletteSampleValue (5))) ;
	addTestResult (genericTestPosition (f2, 95, new BmpPaletteSampleValue (1))) ;
	addTestResult (genericTestPosition (f2, 96, new BmpPaletteSampleValue (8))) ;

	// TODO
}

void BmpOS2FileTest::testReadExtractCompare ()
{
	// TODO
}

void BmpOS2FileTest::testEmbeddedValue ()
{
	Globs = gl1 ; addTestResult (genericTestEmbeddedValue (f1)) ;
	Globs = gl2 ; addTestResult (genericTestEmbeddedValue (f2)) ;
	Globs = gl3 ; addTestResult (genericTestEmbeddedValue (f3)) ;
	Globs = gl4 ; addTestResult (genericTestEmbeddedValue (f4)) ;
}

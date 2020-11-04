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


#include "utcommon.h"
#include "JpegFileTest.h"

JpegFileTest::JpegFileTest (TestSuite* s)
	: CvrStgFileTest("JpegFile", s)
{
	ADDTESTCATEGORY (JpegFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (JpegFileTest, testReadEmbedWriteReadExtract) ;
}

void JpegFileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "std.jpg") ;
	bs1 = new BitString (std::string ("a test passphrase")) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "prog.jpg") ;
	bs2 = new BitString (std::string ("another test passphrase")) ;
	gl2 = Globs ;
}

void JpegFileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete bs1 ; delete bs2 ;
	delete f1 ; delete f2 ;
}

void JpegFileTest::testReadEmbedExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "std.jpg", *bs1)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "prog.jpg", *bs2)) ;
}

void JpegFileTest::testReadEmbedWriteReadExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "std.jpg", *bs2)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "prog.jpg", *bs1)) ;
}

void JpegFileTest::testPosition()
{
	// TODO
}

void JpegFileTest::testReadExtractCompare ()
{
	// TODO
}

void JpegFileTest::testEmbeddedValue ()
{
#if 0
	Globs = gl1 ; addTestResult (genericTestEmbeddedValue (f1)) ;
	Globs = gl2 ; addTestResult (genericTestEmbeddedValue (f2)) ;
	Globs = gl3 ; addTestResult (genericTestEmbeddedValue (f3)) ;
#endif
}

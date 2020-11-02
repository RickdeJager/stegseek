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

#include "AuSampleValues.h"

#include "utcommon.h"
#include "AuFileTest.h"

AuFileTest::AuFileTest (TestSuite* s)
	: CvrStgFileTest("AuFile", s)
{
	ADDTESTCATEGORY (AuFileTest, testReadWrite) ;
	ADDTESTCATEGORY (AuFileTest, testReadEmbedExtract) ;
	ADDTESTCATEGORY (AuFileTest, testReadEmbedWriteReadExtract) ;
	ADDTESTCATEGORY (AuFileTest, testPosition) ;
	ADDTESTCATEGORY (AuFileTest, testReadExtractCompare) ;
	ADDTESTCATEGORY (AuFileTest, testEmbeddedValue) ;
}

void AuFileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;
	bs1 = new BitString (std::string ("a test passphrase")) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "pcm8_std.au") ;
	bs2 = new BitString (std::string ("another test passphrase")) ;
	gl2 = Globs ;

	Globs.reset() ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.au") ;
	bs3 = new BitString (std::string ("inventing test passphrases is boring")) ;
	gl3 = Globs ;

}

void AuFileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete bs1 ; delete bs2 ; delete bs3 ;
	delete f1 ; delete f2 ; delete f3 ;
}

void AuFileTest::testReadWrite()
{
	Globs = gl1 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "mulaw_std.au")) ;
	Globs = gl2 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm8_std.au")) ;
	Globs = gl3 ; addTestResult (genericTestReadWrite (std::string(DATADIR) + "pcm16_std.au")) ;
}

void AuFileTest::testReadEmbedExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "mulaw_std.au", *bs1)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm8_std.au", *bs2)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedExtract (std::string(DATADIR) + "pcm16_std.au", *bs3)) ;
}

void AuFileTest::testReadEmbedWriteReadExtract()
{
	Globs = gl1 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "mulaw_std.au", *bs2)) ;
	Globs = gl2 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm8_std.au", *bs3)) ;
	Globs = gl3 ; addTestResult (genericTestReadEmbedWriteReadExtract (std::string(DATADIR) + "pcm16_std.au", *bs1)) ;
}

void AuFileTest::testPosition()
{
	Globs = gl1 ;
	addTestResult (genericTestPosition (f1, 0, new AuMuLawSampleValue (224))) ;
	addTestResult (genericTestPosition (f1, 10, new AuMuLawSampleValue (148))) ;

	Globs = gl2 ;
	addTestResult (genericTestPosition (f2, 0, new AuPCM8SampleValue (-7))) ;
	addTestResult (genericTestPosition (f2, 1, new AuPCM8SampleValue (-8))) ;
	addTestResult (genericTestPosition (f2, 17, new AuPCM8SampleValue (4))) ;
	addTestResult (genericTestPosition (f2, 81, new AuPCM8SampleValue (-2))) ;

	Globs = gl3 ;
	addTestResult (genericTestPosition (f3, 0, new AuPCM16SampleValue (80))) ;
	addTestResult (genericTestPosition (f3, 2, new AuPCM16SampleValue (-76))) ;
	addTestResult (genericTestPosition (f3, 9, new AuPCM16SampleValue (4403))) ;
	addTestResult (genericTestPosition (f3, 15, new AuPCM16SampleValue (4191))) ;
}

void AuFileTest::testReadExtractCompare ()
{
	{
		Globs = gl1 ;
		BitString shouldbe ;
		shouldbe.append(false).append(true).append(true).append(false).append(false).append(false).append(false).append(true) ;
		addTestResult (genericTestReadExtractCompare (std::string(DATADIR) + "mulaw_std.au", shouldbe)) ;
	}

	{
		Globs = gl2 ;
		// TODO
	}

	{
		Globs = gl3 ;
		// TODO
	}
}

void AuFileTest::testEmbeddedValue ()
{
	Globs = gl1 ; addTestResult (genericTestEmbeddedValue (f1)) ;
	Globs = gl2 ; addTestResult (genericTestEmbeddedValue (f2)) ;
	Globs = gl3 ; addTestResult (genericTestEmbeddedValue (f3)) ;
}

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

#include "BmpPaletteSampleValueTest.h"
#include "utcommon.h"

#include "BmpPaletteSampleValue.h"
#include "CvrStgFile.h"

BmpPaletteSampleValueTest::BmpPaletteSampleValueTest (TestSuite* s)
	: SampleValueTest ("BmpPaletteSampleValue", s)
{
	ADDTESTCATEGORY (BmpPaletteSampleValueTest, testDistance) ;
	ADDTESTCATEGORY (BmpPaletteSampleValueTest, testIsNeighbour) ;
}

void BmpPaletteSampleValueTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "os21x4_std.bmp") ;
	sv1_0 = new BmpPaletteSampleValue (0) ;
	sv1_3 = new BmpPaletteSampleValue (3) ;
	sv1_7 = new BmpPaletteSampleValue (7) ;
	sv1_15 = new BmpPaletteSampleValue (15) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
	sv2_3_49_96_5 = new BmpPaletteSampleValue (3) ;
	sv2_10_11_99_203 = new BmpPaletteSampleValue (10) ;
	sv2_48_18_106_194 = new BmpPaletteSampleValue (48) ;
	sv2_68_73_104_15 = new BmpPaletteSampleValue (68) ;
	sv2_171_61_97_25 = new BmpPaletteSampleValue (171) ;
	gl2 = Globs ;
}

void BmpPaletteSampleValueTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete sv1_0 ; delete sv1_3 ; delete sv1_7 ; delete sv1_15 ;
	delete f1 ;

	delete sv2_3_49_96_5 ; delete sv2_10_11_99_203 ; delete sv2_48_18_106_194 ; delete sv2_68_73_104_15 ; delete sv2_171_61_97_25 ;
	delete f2 ;
}

void BmpPaletteSampleValueTest::testDistance ()
{
	Globs = gl1 ;
	addTestResult (genericTestDistance (sv1_0, sv1_0, 0)) ;
	addTestResult (genericTestDistance (sv1_0, sv1_3, 32768)) ;
	addTestResult (genericTestDistance (sv1_0, sv1_15, 195075)) ;
	addTestResult (genericTestDistance (sv1_3, sv1_7, 16384)) ;

	Globs = gl2 ;
	addTestResult (genericTestDistance (sv2_3_49_96_5, sv2_10_11_99_203, 40657)) ;
	addTestResult (genericTestDistance (sv2_3_49_96_5, sv2_48_18_106_194, 36782)) ;
	addTestResult (genericTestDistance (sv2_3_49_96_5, sv2_68_73_104_15, 740)) ;
	addTestResult (genericTestDistance (sv2_48_18_106_194, sv2_68_73_104_15, 35070)) ;
	addTestResult (genericTestDistance (sv2_171_61_97_25, sv2_10_11_99_203, 34188)) ;
}

void BmpPaletteSampleValueTest::testIsNeighbour ()
{
	Globs = gl1 ;
	addTestResult (genericTestIsNeighbour (sv1_3, sv1_3, true)) ;
	addTestResult (genericTestIsNeighbour (sv1_15, sv1_0, false)) ;
	addTestResult (genericTestIsNeighbour (sv1_7, sv1_15, false)) ;
	addTestResult (genericTestIsNeighbour (sv1_3, sv1_7, false)) ;

	Globs = gl2 ;
	addTestResult (genericTestIsNeighbour (sv2_10_11_99_203, sv2_3_49_96_5, false)) ;
	addTestResult (genericTestIsNeighbour (sv2_68_73_104_15, sv2_171_61_97_25, true)) ;
	addTestResult (genericTestIsNeighbour (sv2_68_73_104_15, sv2_48_18_106_194, false)) ;
	addTestResult (genericTestIsNeighbour (sv2_171_61_97_25, sv2_10_11_99_203, false)) ;
}

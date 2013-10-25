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

#include "utcommon.h"
#include "BmpFileTest.h"

BmpFileTest::BmpFileTest (TestSuite* s)
	: CvrStgFileTest("BmpFile", s)
{
	ADDTESTCATEGORY (BmpFileTest, testSVALCalculation) ;
}

void BmpFileTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs1 = new BitString (std::string ("this BitString will be embedded")) ;
	s1 = new Selector (bs1->getLength() * f1->getSamplesPerVertex(), std::string ("a passphrase")) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "os21x24_std.bmp") ;
	bs2 = new BitString (std::string ("this BitString will be embedded too")) ;
	s2 = new Selector (bs2->getLength() * f2->getSamplesPerVertex(), std::string ("another passphrase")) ;
	g2 = new Graph (f2, *bs2, *s2) ;
	gl2 = Globs ;
}

void BmpFileTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete g1 ; delete g2 ;
	delete s1 ; delete s2 ;
	delete bs1 ; delete bs2 ;
	delete f1 ; delete f2 ;
}

void BmpFileTest::testSVALCalculation ()
{
	Globs = gl1 ; addTestResult (genericTestSVALCalculation (f1, g1)) ;
	Globs = gl2 ; addTestResult (genericTestSVALCalculation (f2, g2)) ;
}

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

#include <vector>

#include "AuSampleValues.h"
#include "common.h"

#include "SampleValueAdjacencyListTest.h"
#include "utcommon.h"

SampleValueAdjacencyListTest::SampleValueAdjacencyListTest (TestSuite* s)
	: UnitTest ("SampleValueAdjacencyList", s), DummySVAL (100)
{
	ADDTESTCATEGORY (SampleValueAdjacencyListTest, testEquality) ;
	ADDTESTCATEGORY (SampleValueAdjacencyListTest, testQuicksort) ;
}

void SampleValueAdjacencyListTest::setup ()
{
	SVEmpty = std::vector<SampleValue*> (100) ;
}

void SampleValueAdjacencyListTest::testEquality ()
{
	CvrStgFile *f = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;

	SampleValueAdjacencyList sval1 (2) ;
	SampleValueAdjacencyList sval2 (2) ;

	sval1[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (10)) ;
	sval1[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (15)) ;
	sval1[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (17)) ;
	sval1[(SampleValueLabel) 1].push_back (new AuMuLawSampleValue (100)) ;
	sval2[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (10)) ;
	sval2[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (15)) ;
	sval2[(SampleValueLabel) 0].push_back (new AuMuLawSampleValue (17)) ;
	sval2[(SampleValueLabel) 1].push_back (new AuMuLawSampleValue (100)) ;

	addTestResult (sval1 == sval2) ;

	sval2[(SampleValueLabel) 1].push_back (new AuMuLawSampleValue (110)) ;

	addTestResult (!(sval1 == sval2)) ;

	delete f ;
}

void SampleValueAdjacencyListTest::testQuicksort ()
{
	{
		UWORD32* d = new UWORD32[2] ;
		d[0] = 3 ;
		d[1] = 5 ;
		DummySVAL.quicksort (SVEmpty, d, 0, 1) ;
		addTestResult ((d[0] == 3) && (d[1] == 5)) ;
		delete[] d ;
	}

	{
		UWORD32* d = new UWORD32[2] ;
		d[0] = 5 ;
		d[1] = 3 ;
		DummySVAL.quicksort (SVEmpty, d, 0, 1) ;
		addTestResult ((d[0] == 3) && (d[1] == 5)) ;
		delete[] d ;
	}

	{
		UWORD32* d = new UWORD32[3] ;
		d[0] = 2 ;
		d[1] = 2 ;
		d[2] = 2 ;
		DummySVAL.quicksort (SVEmpty, d, 0, 2) ;
		addTestResult ((d[0] == 2) && (d[1] == 2) && (d[2] == 2)) ; // asserting termination
		delete[] d ;
	}

	{
		UWORD32* d = new UWORD32[10] ;
		d[0] = 3 ; d[1] = 12 ; d[2] = 8 ; d[3] = 2 ; d[4] = 22 ;
		d[5] = 7 ; d[6] = 8 ; d[7] = 4 ; d[8] = 6 ; d[9] = 17 ;
		DummySVAL.quicksort (SVEmpty, d, 0, 9) ;
		addTestResult ((d[0] == 2) && (d[1] == 3) && (d[2] == 4) && (d[3] == 6) && (d[4] == 7) &&
					   (d[5] == 8) && (d[6] == 8) && (d[7] == 12) && (d[8] == 17) && (d[9] == 22)) ;
		delete[] d ;
	}
}

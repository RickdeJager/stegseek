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

#include "common.h"

#include "AUtilsTest.h"
#include "TestCategoryCaller.h"

AUtilsTest::AUtilsTest (TestSuite* s)
	: UnitTest ("AUtils", s)
{
	ADDTESTCATEGORY (AUtilsTest, testLog2) ;
}

void AUtilsTest::testLog2 ()
{
	addTestResult (genericTestLog2 (2, 1.0)) ;
	addTestResult (genericTestLog2 (8, 3.0)) ;
	addTestResult (genericTestLog2 (5, 2.321928)) ;
	addTestResult (genericTestLog2 (21, 4.392317)) ;
}

bool AUtilsTest::genericTestLog2 (int n, double res)
{
	// to avoid failing because of numeric problems
	double tmp = log2 (n) ;
	double lb = res - 0.000001 ;
	double ub = res + 0.000001 ;
	return (lb < tmp && tmp < ub) ;
}

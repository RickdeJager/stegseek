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

#include "TestSuite.h"

TestSuite::TestSuite()
	: Test()
{
	runningUnit = "__none__" ;
	runningCategory = "__none__" ;
	TestNumber = 0 ;
	OutStream = &std::cout ;
}

void TestSuite::addUnitTest (UnitTest* ut)
{
	UnitTests.push_back (ut) ;
}

void TestSuite::run()
{
	SuiteOk = true ;
	*OutStream << "-----------------------" << std::endl << std::endl ;
	*OutStream << "Running Unit Tests....." << std::endl << std::endl ;
	*OutStream << "-----------------------" << std::endl << std::endl ;
	for (std::vector<UnitTest*>::iterator it = UnitTests.begin() ; it != UnitTests.end() ; it++) {
		(*it)->setup() ;
		(*it)->run() ;
		(*it)->cleanup() ;
	}
}

void TestSuite::startUnit (std::string n)
{
	if (runningUnit == "__none__") {
		*OutStream << n << ":" << std::endl ;
		runningUnit = n ;
	}
	else {
		*OutStream << "UNEXPECTED ERROR in " << __FILE__ << ":" << __LINE__ << std::endl ;
	}
}

void TestSuite::endUnit (std::string n)
{
	if (runningUnit == n) {
		*OutStream << std::endl ;
		runningUnit = "__none__" ;
	}
	else {
		*OutStream << "UNEXPECTED ERROR in " << __FILE__ << ":" << __LINE__ << std::endl ;
	}
}


void TestSuite::startCategory (std::string n)
{
	if (runningCategory == "__none__") {
		*OutStream << "    " << n ;
		runningCategory = n ;
		runningCategoryOk = true ;
		OutStream->flush() ;
	}
	else {
		*OutStream << "UNEXPECTED ERROR in " << __FILE__ << ":" << __LINE__ << std::endl ;
	}
}

void TestSuite::endCategory (std::string n)
{
	if (runningCategory == n) {
		if (runningCategoryOk) {
			*OutStream << "ok" << std::endl ;
		}
		else {
			*OutStream << " !! FAILED !!" << std::endl ;
		}
		runningCategory = "__none__" ;
	}
	else {
		*OutStream << "UNEXPECTED ERROR in " << __FILE__ << ":" << __LINE__ << std::endl ;
	}
}

void TestSuite::addTestResult (bool r)
{
	if (runningUnit == "__none__" || runningCategory == "__none__") {
		*OutStream << "UNEXPECTED ERROR in " << __FILE__ << ":" << __LINE__ << std::endl ;
	}
	else {
		if (r) {
			*OutStream << "." ;
		}
		else {
			*OutStream << "F" ;
			SuiteOk = false ;
		}
		runningCategoryOk &= r ;
		OutStream->flush() ;
	}
}

void TestSuite::addTestResult (TestSuite::TESTRESULT r)
{
	switch (r) {
		case NOTINSTALLED:
		*OutStream << "not installed" << std::endl ;
		runningCategory = "__none__" ;
		break ;

		case KNOWNEXCEPTION:
		*OutStream << "caught known exception:" << std::endl ;
		runningCategory = "__none__" ;
		break ;

		case UNKNOWNEXCEPTION:
		*OutStream << "caugh unknown exception (!!)" << std::endl ;
		runningCategory = "__none__" ;
		break ;
	}
}

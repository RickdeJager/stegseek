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

#ifndef SH_TESTSUITE_H
#define SH_TESTSUITE_H

#include <string>
#include <vector>
#include <iostream>

#include "Test.h"
#include "UnitTest.h"

class TestSuite : public Test {
	public:
	enum TESTRESULT {	NOTINSTALLED,	// when something that is needed for the test is not installed on the current system
						KNOWNEXCEPTION,
						UNKNOWNEXCEPTION } ;

	TestSuite (void) ;

	void addUnitTest (UnitTest* ut) ;

	void run (void) ;

	/**
	 * \return true iff all tests were ok
	 **/
	bool getResult (void)
		{ return SuiteOk ; } ;

	// used during testing
	void startUnit (std::string n) ;
	void endUnit (std::string n) ;
	void startCategory (std::string n) ;
	void endCategory (std::string n) ;
	void addTestResult (bool r) ;
	void addTestResult (TESTRESULT r) ;

	private:
	std::vector<UnitTest*> UnitTests ;

	std::string runningUnit ;
	std::string runningCategory ;
	bool runningCategoryOk ;
	bool SuiteOk ;
	unsigned short TestNumber ;
	std::ostream* OutStream ;
} ;

#endif // ndef SH_TESTSUITE_H

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

#ifndef SH_UNITTEST_H
#define SH_UNITTEST_H

#include <string>
#include <vector>

#include "common.h"

#include "Test.h"
#include "TestCategoryCaller.h"
// declared here to avoid circulating includes
class TestCategory ;
class TestSuite ;

#define ADDTESTCATEGORY(UTCLASS,CMETHOD) \
	addTestCategory (new TestCategoryCaller<UTCLASS> (strip_test(#CMETHOD), getSuite(), this, &UTCLASS::CMETHOD))

class UnitTest : public Test {
	public:
	/**
	 * \param n name of this unit test (probably the name of the tested class)
	 **/
	UnitTest (std::string n, TestSuite* s) : Test(n,s), GlobsBackup(NULL) {} ;

	~UnitTest (void) ;

	/**
	 * setup the unit test - called before run
	 *
	 * UnitTest::setup will (together with UnitTest::cleanup) save and restore the object
	 * stored in Globs so they should be called from the corresponding functions in the
	 * derived object if the derived unit test manipulates the Globs object.
	 **/
	virtual void setup (void) ;

	void run (void) ;

	/**
	 * cleanup the unit test - called after run
	 **/
	virtual void cleanup (void) ;

	protected:
	void addTestCategory (TestCategory *tc) ;

	void addTestResult (bool r) ;

	char *strip_test (char* s)
		{ return (s + 4) ; } ;

	private:
	std::vector<TestCategory*> TestCategories ;

	/// servers as a backup for the Globals object stored in Globs
	Globals GlobsBackup ;
} ;

#endif // ndef SH_UNITTEST_H

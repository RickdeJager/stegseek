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

#include "Selector.h"

#include "SelectorTest.h"
#include "TestCategoryCaller.h"

SelectorTest::SelectorTest (TestSuite* s)
	: UnitTest ("Selector", s)
{
	ADDTESTCATEGORY (SelectorTest, testIsPermutation) ;
	ADDTESTCATEGORY (SelectorTest, testIsIdentityPermutation) ;
}

void SelectorTest::setup ()
{
	s1 = new Selector (10, "a passphrase") ;
	s2 = new Selector (50, "another passphrase") ;
	s3 = new Selector (10000, "a large Selector object") ;
	s4 = new Selector (128, "a smaller object again") ;
	sid1 = new Selector (16) ;
	sid2 = new Selector (1234) ;
}

void SelectorTest::cleanup ()
{
	delete s1 ; delete s2 ; delete s3 ; delete s4 ;
	delete sid1 ; delete sid2 ;
}

void SelectorTest::testIsPermutation ()
{
	addTestResult (genericTestIsPermutation (s1)) ;
	addTestResult (genericTestIsPermutation (s2)) ;
	addTestResult (genericTestIsPermutation (s3)) ;
	addTestResult (genericTestIsPermutation (s4)) ;
}

void SelectorTest::testIsIdentityPermutation ()
{
	addTestResult (genericTestIsIdentityPermutation (sid1)) ;
	addTestResult (genericTestIsIdentityPermutation (sid2)) ;
}

bool SelectorTest::genericTestIsPermutation (Selector* s)
{
	std::vector<bool> hasoccured (s->getRange()) ; // all set to false

	bool range_ok = true ;
	bool inj = true ;
	for (UWORD32 i = 0 ; i < s->getRange() ; i++) {
		UWORD32 value = (*s)[i] ;
		range_ok = (value < s->getRange()) && range_ok ;
		inj = !hasoccured[value] && inj ;
		hasoccured[value] = true ;
	}

	bool surj = true ;
	for (UWORD32 i = 0 ; i < s->getRange() ; i++) {
		surj = hasoccured[i] && surj ;
	}

	return range_ok && inj && surj ;
}

bool SelectorTest::genericTestIsIdentityPermutation (Selector* s)
{
	bool ok = true ;
	for (UWORD32 i = 0 ; i < s->getRange() ; i++) {
		ok = ((*s)[i] == i) && ok ;
	}
	return ok ;
}

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

#ifndef SH_TEST_H
#define SH_TEST_H

#include <string>

class TestSuite ;

class Test {
	public:
	Test (void) : Name(std::string("unnamed test")), Suite(NULL) {} ;
	Test (const std::string& n) : Name(n), Suite(NULL) {} ;
	Test (const std::string& n, TestSuite* s) : Name(n), Suite(s) {} ;

	virtual ~Test (void) {} ;

	const std::string& getName (void)
		{ return Name ; } ;

	TestSuite* getSuite (void)
		{ return Suite ; } ;

	virtual void run (void) = 0 ;

	private:
	std::string Name ;
	TestSuite* Suite ;
} ;

#endif // ndef SH_TEST_H

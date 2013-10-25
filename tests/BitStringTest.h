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

#ifndef SH_BITSTRINGTEST_H
#define SH_BITSTRINGTEST_H

#define private public
#define protected public
#include "BitString.h"
#undef private
#undef protected

#include "UnitTest.h"
#include "TestSuite.h"

class BitStringTest : public UnitTest {
	public:
	BitStringTest (TestSuite* s) ;

	void setup (void) ;
	void cleanup (void) ;

	void testBitInputOutput (void) ;
	void testLength (void) ;
	void testDatatypeInput (void) ;
	void testDatatypeOutput (void) ;
	void testEquality (void) ;
	void testCutting (void) ;
	void testCompression (void) ;
	void testArity (void) ;

	private:
	BitString *bs_0, *bs_1, *bs_10, *bs_001, *bs_100, *bs_1010, *bs_1110, *bs_01011, *bs_10010, *bs_10101110, *bs_101011101 ;
} ;

#endif // ndef SH_BITSTRINGTEST_H

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

#ifndef SH_MHASHPPTEST_H
#define SH_MHASHPPTEST_H

#define private public
#define protected public
#include "MHashPP.h"
#undef private
#undef protected

#include "BitString.h"
#include "UnitTest.h"
#include "TestSuite.h"

class MHashPPTest : public UnitTest {
	public:
	MHashPPTest (TestSuite* s) ;

	/**
	 * test output of MHashPP class with MD5 algorithm against the test vectors given in RFC 1321
	 **/
	void testMD5 (void) ;

	void testCRC32 (void) ;

	private:
	/**
	 * compute a hash using MHashPP and compare it to a reference value
	 * \param a the hash algorithm that should be used by MHashPP
	 * \param data the input of the hash algorithm
	 * \param shouldbe the reference value of the result of the hash algorithm
	 * \return true iff shouldbe is the result of the hash algorithm a applied to data by MHashPP
	 **/
	bool genericTestMHashPP (hashid a, BitString data, BYTE* shouldbe) ;
} ;

#endif // ndef SH_MHASHPPTEST_H

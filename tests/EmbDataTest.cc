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

#include "BitString.h"

#include "EmbDataTest.h"
#include "TestCategoryCaller.h"
#include "utcommon.h"

EmbDataTest::EmbDataTest (TestSuite* s)
	: UnitTest ("EmbData", s)
{
	ADDTESTCATEGORY (EmbDataTest, testEmbedding) ;
	ADDTESTCATEGORY (EmbDataTest, testExtracting) ;
}

void EmbDataTest::testEmbedding ()
{
	{
		std::vector<BYTE> data (1) ;
		EmbData emb (EmbData::EMBED, "passphrase not used", "test.txt") ;
		emb.setEncAlgo (EncryptionAlgorithm (EncryptionAlgorithm::NONE)) ;
		emb.setEncMode (EncryptionMode (EncryptionMode::ECB)) ;
		emb.setCompression (0) ;
		emb.setChecksum (false) ;
		emb.setData (data) ;

		BitString shouldbe ;
		shouldbe.append (EmbData::Magic, EmbData::NBitsMagic) ; // magic
		shouldbe.append (false) ; // version
		shouldbe.append ((BYTE) 0, 5) ; // encalgo
		shouldbe.append ((BYTE) 0, 3) ; // encmode
		shouldbe.append ((UWORD32) 82, 32) ; // nplainbits
		shouldbe.append (false) ; // compression
		shouldbe.append (false) ; // checksum
		shouldbe.append (std::string("test.txt")) ; // filename
		shouldbe.append ((BYTE) 0, 8) ; // end of filename
		shouldbe.append ((BYTE) 0, 8) ; // data

		addTestResult (genericTestEmbedding (emb, shouldbe)) ;
	}
}

void EmbDataTest::testExtracting ()
{
	{
		BitString bs ;
		bs.append (EmbData::Magic, EmbData::NBitsMagic) ; // magic
		bs.append (false) ; // version
		bs.append ((BYTE) 0, 5) ; // encalgo
		bs.append ((BYTE) 1, 3) ; // encmode
		bs.append ((UWORD32) 82, 32) ; // nplainbits
		bs.append (false) ; // compression
		bs.append (false) ; // checksum
		bs.append (std::string("testfile")) ; // filename
		bs.append ((BYTE) 0, 8) ; // end of filename
		bs.append ((BYTE) 0xff, 8) ; // data

		EmbData emb (EmbData::EXTRACT, "passphrase not used") ;
		bool ok = feed_to (bs, emb) ;

		ok = (emb.getEncAlgo() == EncryptionAlgorithm(EncryptionAlgorithm::NONE)) && ok ;
		ok = (emb.getEncMode() == EncryptionMode(EncryptionMode::CBC)) && ok ;
		ok = (emb.getCompression() == 0) && ok ;
		ok = (emb.getChecksum() == false) && ok ;
		ok = (emb.getFileName() == "testfile") && ok ;
		ok = (emb.getData().size() == 1 && emb.getData()[0] == 0xff) && ok ;

		addTestResult (ok) ;
	}
}

bool EmbDataTest::genericTestEmbedding (EmbData e, BitString shouldbe)
{
	BitString res = e.getBitString() ;
	bool retval = (res == shouldbe) ;
	if (ArgVerbose && !retval) {
		std::cerr << std::endl << "---- FAILED: genericTestEmbedding ----" << std::endl ;
		std::cerr << "result:" << std::endl ;
		res.print() ;
		std::cerr << "should be:" << std::endl ;
		shouldbe.print() ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	return retval ;
}

bool EmbDataTest::feed_to (const BitString& bs, EmbData& emb)
{
	UWORD32 idx = 0 ;
	while (!emb.finished()) {
		UWORD32 bitsrequested = emb.getNumBitsRequested() ;
		if (idx + bitsrequested > bs.getLength()) {
			return false ;
		}

		emb.addBits (bs.getBits (idx, bitsrequested)) ;
		idx += bitsrequested ;
	}

	return (idx == bs.getLength()) ;
}

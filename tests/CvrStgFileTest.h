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

#ifndef SH_CVRSTGFILETEST_H
#define SH_CVRSTGFILETEST_H

#include "BitString.h"
#include "CvrStgFile.h"

#include "UnitTest.h"
#include "TestSuite.h"

class CvrStgFileTest : public UnitTest {
	public:
	CvrStgFileTest (std::string n, TestSuite* s) : UnitTest(n,s) {} ;

	protected:
	/**
	 * read the file rfn and write it again, return true iff write op created what was read by read op
	 * \param rfn the file name of the file to read
	 * \param new_wfn wether to write the data to another file or to rfn (a backup copy is done)
	 **/
	bool genericTestReadWrite (const std::string& rfn, bool new_wfn = true) const ;
	bool genericTestReadEmbedExtract (const std::string& fn, BitString& emb) const ;

	/**
	 * read fn and extract the first emb.getLength() bits, return true iff these equal emb
	 **/
	bool genericTestReadExtractCompare (const std::string& fn, BitString& emb) const ;
	bool genericTestReadEmbedWriteReadExtract (const std::string& cvrfn, BitString& emb) const ;
	bool genericTestPosition (const CvrStgFile* f, const SamplePos pos, SampleValue* sv_r) const ;
	bool genericTestSVALCalculation (const CvrStgFile* f, const Graph* g) const ;

	/**
	 * for all sample positions, test if f->getEmbeddedValue(p) and f->getSampleValue(p)->getEmbeddedValue() return the same result
	 **/
	bool genericTestEmbeddedValue (const CvrStgFile* f) const ;

	private:
	bool areEqual (const std::string& fn1, const std::string& fn2) const ;
	void removeFile (const std::string& fn) const ;
	void copyFile (const std::string& src, const std::string& dest) const ;
	void moveFile (const std::string& src, const std::string& dest) const ;
} ;

#endif // ndef SH_CVRSTGFILETEST_H

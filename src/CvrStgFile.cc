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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "DFSAPHeuristic.h"
#include "AuFile.h"
#include "AUtils.h"
#include "BmpFile.h"
#include "CvrStgFile.h"
#include "JpegFile.h"
#include "SampleValue.h"
#include "SampleValueAdjacencyList.h"
#include "Utils.h"
#include "WavFile.h"
#include "WKSConstructionHeuristic.h"
#include "common.h"
#include "error.h"
#include "msg.h"

CvrStgFile::CvrStgFile ()
	: BinIO(NULL), SamplesPerVertex(0), Radius(0), EmbValueModulus(0)
{
	Globs.TheCvrStgFile = this ;
}

CvrStgFile::~CvrStgFile (void)
{
	if (BinIO != NULL) {
		delete BinIO ;
	}
}

void CvrStgFile::setRadius (UWORD32 r)
{
	if (Args.Radius.is_set()) {
		Radius = Args.Radius.getValue() ;
	}
	else {
		Radius = r ;
	}
}

void CvrStgFile::read (BinaryIO *io)
{
	setBinIO (io) ;
}

void CvrStgFile::write (void)
{
}

void CvrStgFile::transform (const std::string& stgfn)
{
	delete BinIO ;
	BinIO = NULL ;
	BinIO = new BinaryIO (stgfn, BinaryIO::WRITE) ;
}

std::vector<SampleValueAdjacencyList*> CvrStgFile::calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const
{
	EmbValue m = getEmbValueModulus() ;
	std::vector<SampleValueAdjacencyList*> lists (m) ;
	for (EmbValue i = 0 ; i < m ; i++) {
		lists[i] = new SampleValueAdjacencyList (svs.size()) ;
	}

	for (SampleValueLabel i = 0 ; i < svs.size() ; i++) { // to be integrated into lists
		SampleValueAdjacencyList* sval = lists[svs[i]->getEmbeddedValue()] ;
		for (SampleValueLabel j = 0 ; j < svs.size() ; j++) { // line where integration should happen
			if ((svs[i]->isNeighbour(svs[j])) && (i != j)) {
				(*sval)[svs[j]].push_back (svs[i]) ;
			}
		}
	}

	for (EmbValue i = 0 ; i < m ; i++) {
		lists[i]->sort() ;
	}

	return lists ;
}

std::vector<MatchingAlgorithm*> CvrStgFile::getMatchingAlgorithms (Graph* g, Matching* m) const
{
	std::vector<MatchingAlgorithm*> retval ;
	retval.push_back (new WKSConstructionHeuristic (g, m)) ;
	retval.push_back (new DFSAPHeuristic (g, m)) ;
	return retval ;
}

EmbValue CvrStgFile::getEmbeddedValue (const SamplePos pos) const
{
	SampleValue* sv = getSampleValue(pos) ;
	EmbValue retval = sv->getEmbeddedValue() ;
	delete sv ;
	return retval ;
}

unsigned long CvrStgFile::getCapacity () const
{
	float maxnvertices = getNumSamples() / getSamplesPerVertex() ;
	float maxnbits = maxnvertices * log2(getEmbValueModulus()) ;
	return ((unsigned long) (maxnbits / 8)) ;
}

std::string CvrStgFile::getHRCapacity () const
{
	return Utils::formatHRSize (getCapacity()) ;
}

#ifdef DEBUG
std::map<SampleKey,unsigned long>* CvrStgFile::getFrequencies ()
{
	unsigned long n = getNumSamples() ;
	std::map<SampleKey,unsigned long>* table = new std::map<SampleKey,unsigned long> () ;

	for (unsigned long pos = 0 ; pos < n ; pos++) {
		SampleValue *sv = getSampleValue (pos) ;
		(*table)[sv->getKey()]++ ;
		delete sv ;
	}

	return table ;
}

void CvrStgFile::printFrequencies (const std::map<SampleKey,unsigned long>& freqs)
{
	for (std::map<SampleKey,unsigned long>::const_iterator fit = freqs.begin() ; fit != freqs.end() ; fit++) {
		std::cout << fit->first << ": " << fit->second << std::endl ;
	}
}

#endif // def DEBUG

CvrStgFile::FILEFORMAT CvrStgFile::guessff (BinaryIO *io)
{
	char buf[4] = { '\0', '\0', '\0', '\0' } ;
	FILEFORMAT retval = UNKNOWN ;
	
	for (unsigned int i = 0 ; i < 2 ; i++) {
		buf[i] = (char) io->read8() ;
	}

	if (strncmp ("BM", buf, 2) == 0) {
		retval = BMP ;
	}
	else if ((unsigned char) buf[0] == 0xFF && (unsigned char) buf[1] == 0xD8) {
		retval = JPEG ;
	}
	else {
		for (unsigned int i = 2 ; i < 4 ; i++) {
			buf[i] = (char) io->read8() ;
		}

		if (strncmp (".snd", buf, 4) == 0) {
			retval = AU ;
		}
		else if (strncmp ("RIFF", buf, 4) == 0) {
			retval = WAV ;
		}
	}

	return retval ;
}

CvrStgFile* CvrStgFile::readFile (const std::string& fn)
{
	BinaryIO *BinIO = new BinaryIO (fn, BinaryIO::READ) ;

	CvrStgFile *file = NULL ;
	switch (guessff (BinIO)) {
		case UNKNOWN: {
			throw UnSupFileFormat (BinIO) ;	
		break ; }

		case BMP: {
			file = new BmpFile (BinIO) ;
		break ; }

		case WAV: {
			file = new WavFile (BinIO) ;
		break ; }

		case AU: {
			file = new AuFile (BinIO) ;
		break ; }

		case JPEG: {
#ifdef USE_LIBJPEG
			file = new JpegFile (BinIO) ;
#else
			throw SteghideError (_("can not read input file. steghide has been compiled without support for jpeg files.")) ;
#endif
		break ; }

		default: {
			myassert(0) ;
		break ; }
	}

	return file ;
}

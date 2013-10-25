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

#include "AUtils.h"
#include "BFSAPHeuristic.h"
#include "ColorPalette.h"
#include "CvrStgFile.h"
#include "DFSAPHeuristic.h"
#include "DMDConstructionHeuristic.h"
#include "BmpFile.h"
#include "BmpPaletteSampleValue.h"
#include "BmpRGBSampleValue.h"
#include "SampleValueAdjacencyList.h"
#include "SMDConstructionHeuristic.h"
#include "WKSConstructionHeuristic.h"
#include "common.h"
#include "error.h"

BmpFile::BmpFile (BinaryIO *io)
	: CvrStgFile()
{
	read (io) ;
}

BmpFile::~BmpFile ()
{
	delete Palette ;
}

BmpFile::SUBFORMAT BmpFile::getSubformat () const
{
	return subformat ;
}

void BmpFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;
	Palette = NULL ;

	readheaders() ;
	readdata() ;
}

void BmpFile::write ()
{
	CvrStgFile::write() ;

	writeheaders() ;
	writedata() ;
}

std::list<CvrStgFile::Property> BmpFile::getProperties () const
{
	std::list<CvrStgFile::Property> retval ;

	// format
	std::string formatstring ;
	switch (getSubformat()) {
		case WIN:
			formatstring = _("Windows 3.x bitmap") ;
		break ;

		case OS2:
			formatstring = _("OS/2 1.x bitmap") ;
		break ;
	}
	retval.push_back (CvrStgFile::Property (_("format"), formatstring)) ;

	return retval ;
}

std::vector<MatchingAlgorithm*> BmpFile::getMatchingAlgorithms (Graph* g, Matching* m) const
{
	std::vector<MatchingAlgorithm*> retval ;
	if (getBitCount() == 24) {
		retval.push_back (new SMDConstructionHeuristic (g, m)) ;
	}
	else {
		retval.push_back (new SMDConstructionHeuristic (g, m)) ;
	}
	return retval ;
}

unsigned long BmpFile::getNumSamples() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biWidth * bmih.biHeight ;
		break ; }

		case OS2: {
			retval = bmch.bcWidth * bmch.bcHeight ;
		break ; }
	}
	return retval ;
}

void BmpFile::replaceSample (const SamplePos pos, const SampleValue* s)
{
	unsigned long index = 0 ;
	unsigned short firstbit = 0 ;
	calcIndex (pos, &index, &firstbit) ;

	unsigned short bitcount = getBitCount() ;
	switch (bitcount) {
		case 1: case 4: case 8: {
			const BmpPaletteSampleValue* sample = dynamic_cast<const BmpPaletteSampleValue*> (s) ;
			myassert (sample) ;

			for (unsigned short i = 0 ; i < bitcount ; i++) {
				BitmapData[index] = BitmapData[index] & (~(1 << (firstbit + i))) ;
				BitmapData[index] = BitmapData[index] | ((sample->getIndex() & (1 << i)) << firstbit) ;
			}
		break ; }

		case 24: {
			const BmpRGBSampleValue* sample = dynamic_cast<const BmpRGBSampleValue*> (s) ;
			myassert (sample) ;

			BitmapData[index] = sample->getBlue() ;
			BitmapData[index + 1] = sample->getGreen() ;
			BitmapData[index + 2] = sample->getRed() ;
		break ; }
	}
}

SampleValue *BmpFile::getSampleValue (SamplePos pos) const
{
	unsigned long index = 0 ;
	unsigned short firstbit = 0 ;
	calcIndex (pos, &index, &firstbit) ;

	unsigned short bitcount = getBitCount() ;
	SampleValue *retval = NULL ;
	switch (bitcount) {
		case 1: case 4: case 8: {
			BYTE idx_pal = 0 ;
			for (unsigned short i = 0 ; i < bitcount ; i++) {
				idx_pal |= ((BitmapData[index] & (1 << (firstbit + i))) >> firstbit) ;
			}
			retval = (SampleValue*) new BmpPaletteSampleValue (idx_pal) ;
		break ; }

		case 24: {
			retval = (SampleValue*) new BmpRGBSampleValue (BitmapData[index + 2], BitmapData[index + 1], BitmapData[index]) ;
		break ; }
	}
	return retval ;
}

std::vector<SampleValueAdjacencyList*> BmpFile::calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const
{
	if (getBitCount() == 24) {
		EmbValue m = getEmbValueModulus() ;
		UWORD32 r = getRadius() ;

		// create svalists
		std::vector<SampleValueAdjacencyList*> lists (m) ;
		for (EmbValue i = 0 ; i < m ; i++) {
			lists[i] = new SampleValueAdjacencyList (svs.size()) ;
		}

		// redmap contains all sample values in a 3-dimensional RGB-tree
		typedef std::map<BYTE, BmpRGBSampleValue*>	BlueMap ;
		typedef std::map<BYTE, BlueMap>				GreenMap ;
		typedef std::map<BYTE, GreenMap>			RedMap ;
		RedMap redmap ;
		for (std::vector<SampleValue*>::const_iterator svit = svs.begin() ; svit != svs.end() ; svit++) {
			BmpRGBSampleValue* rgbsv = (BmpRGBSampleValue*) (*svit) ;
			redmap[rgbsv->getRed()][rgbsv->getGreen()][rgbsv->getBlue()] = rgbsv ;
		}

		// create hemisphere matrix (will contain a discrete hemisphere with radius getRadius())
		unsigned short r_eucl = (unsigned short) sqrt (r) ; // the euclidean radius (not squared) - rounded to next lower natural number 
		short hemisphere[2 * r_eucl + 1][2 * r_eucl + 1] ;
		for (short dr = -r_eucl ; dr <= r_eucl ; dr++) {
			for (short dg = -r_eucl ; dg <= r_eucl ; dg++) {
				short db_sq = r_eucl*r_eucl - dr*dr - dg*dg ;
				if (db_sq >= 0) {
					// round hemishpere points to next lower natural numbers
					hemisphere[dr + r_eucl][dg + r_eucl] = (short) sqrt (db_sq) ;
				}
				else {
					hemisphere[dr + r_eucl][dg + r_eucl] = -1 ;
				}
			}
		}

		//
		// fill adjacency lists
		//

		// create reservoir - for every i reservoir[i] contains the sample values that are neighbourss of
		// the sample value with label i and have a lower label (and have already been found)
		// This is necessary to use collapsing trees together with bucket sort (without huge increase in memory usage)
		std::vector<BmpRGBSampleValue*> reservoir[svs.size()] ;

		// neighbours sorted by distance (for the current source sample value)
		std::vector<BmpRGBSampleValue*> neighbours_byd[r + 1] ;

		for (std::vector<SampleValue*>::const_iterator srcsvit = svs.begin() ; srcsvit != svs.end() ; srcsvit++) {
			BmpRGBSampleValue* srcsv = (BmpRGBSampleValue*) (*srcsvit) ;

			// sort reservoir into neighbours_byd
			for (std::vector<BmpRGBSampleValue*>::const_iterator it = reservoir[srcsv->getLabel()].begin() ;
					it != reservoir[srcsv->getLabel()].end() ; it++) {
				neighbours_byd[srcsv->calcDistance(*it)].push_back (*it) ;
			}

			unsigned short r_abs_start = AUtils::bminus<unsigned short> (srcsv->getRed(), r_eucl) ;
			unsigned short r_abs_end = AUtils::bplus<unsigned short, 255> (srcsv->getRed(), r_eucl) ;
			unsigned short g_abs_start = AUtils::bminus<unsigned short> (srcsv->getGreen(), r_eucl) ;
			unsigned short g_abs_end = AUtils::bplus<unsigned short, 255> (srcsv->getGreen(), r_eucl) ;

			RedMap::iterator rit = redmap.lower_bound (r_abs_start) ;
			while ((rit != redmap.end()) && (rit->first <= r_abs_end)) {
				GreenMap& greenmap = rit->second ;
				
				if (greenmap.empty()) {
					RedMap::iterator delme = rit ;
					rit++ ;
					redmap.erase (delme) ;
				}
				else {
					GreenMap::iterator git = greenmap.lower_bound (g_abs_start) ;
					while ((git != greenmap.end()) && (git->first <= g_abs_end)) {
						unsigned short red_index = (rit->first - srcsv->getRed()) + r_eucl ;
						unsigned short green_index = (git->first - srcsv->getGreen()) + r_eucl ;
						short delta_b = hemisphere[red_index][green_index] ;

						if (delta_b < 0) { // this blue map has no sample values in sphere
							git++ ;
						}
						else {
							BlueMap& bluemap = git->second ;
							if (bluemap.empty()) {
								GreenMap::iterator delme = git ;
								git++ ;
								greenmap.erase (delme) ;
							}
							else {
								unsigned short b_abs_start = AUtils::bminus<unsigned short> (srcsv->getBlue(), delta_b) ;
								unsigned short b_abs_end = AUtils::bplus<unsigned short,255> (srcsv->getBlue(), delta_b) ;
								BlueMap::iterator bit = bluemap.lower_bound (b_abs_start) ;
								while ((bit != bluemap.end()) && (bit->first <= b_abs_end)) {
									if (srcsv->getLabel() < bit->second->getLabel()) {
										neighbours_byd[srcsv->calcDistance(bit->second)].push_back (bit->second) ;
										reservoir[bit->second->getLabel()].push_back (srcsv) ;
										bit++ ;
									}
									else {
										BlueMap::iterator delme = bit ;
										bit++ ;
										bluemap.erase (delme) ;
									}
								}
								git++ ;
							}
						}
					}	// end of git loop

					rit++ ;
				}
			}	// end of rit loop

			for (unsigned short d = 0 ; d <= r ; d++) {
				if (!neighbours_byd[d].empty()) {
					for (std::vector<BmpRGBSampleValue*>::const_iterator it = neighbours_byd[d].begin() ;
							it != neighbours_byd[d].end() ; it++) {
						(*(lists[(*it)->getEmbeddedValue()]))[srcsv].push_back (*it) ;
					}
					neighbours_byd[d].clear() ;
				}
			}
		} // end of svs for loop

		return lists ;
	}
	else {
		return CvrStgFile::calcSVAdjacencyLists(svs) ;
	}
}

void BmpFile::calcIndex (SamplePos pos, unsigned long* index, unsigned short* firstbit) const
{
	unsigned long width = getWidth(), bitcount = getBitCount() ;

	unsigned long bytesperline_nonpadded = 0 ;
	if (width * bitcount % 8 == 0) {
		bytesperline_nonpadded = (width * bitcount) / 8 ;
	}
	else {
		bytesperline_nonpadded = (width * bitcount) / 8 + 1 ;
	}

	unsigned long row = pos / width ;
	pos = pos % width ;

	unsigned long column = 0 ;
	switch (bitcount) {
		case 1: case 4: case 8:
		{
			unsigned short samplesperbyte = 8 / bitcount ;

			column = pos / samplesperbyte ;

			// to account for the order pixels are stored in one byte:
			*firstbit = (samplesperbyte - (pos % samplesperbyte) - 1) * bitcount ;

			myassert (*firstbit < 8) ;
		}
		break ;

		case 24:
			column = pos * 3 ;
			*firstbit = 0 ;
		break ;
		
		default:
			myassert(false) ;
		break ;
	}

	*index = bytesperline_nonpadded * row + column ;
}

unsigned short BmpFile::getBitCount() const
{
	unsigned short retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biBitCount ;
		break ; }

		case OS2: {
			retval = bmch.bcBitCount ;
		break ; }
	}
	myassert (retval == 1 || retval == 4 || retval == 8 || retval == 24) ;
	return retval ;
}

unsigned long BmpFile::getWidth() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN:
			retval = bmih.biWidth ;
		break ;

		case OS2:
			retval = bmch.bcWidth ;
		break ;
	}
	return retval ;
}

unsigned long BmpFile::getHeight() const
{
	unsigned long retval = 0 ;
	switch (getSubformat()) {
		case WIN: {
			retval = bmih.biHeight ;
		break ; }

		case OS2: {
			retval = bmch.bcHeight ;
		break ; }
	}

	return retval ;
}

ColorPalette *BmpFile::getPalette() const
{
	myassert (getBitCount() != 24) ;
	myassert (Palette) ;
	return Palette ;
}

/* reads the headers of a bmp file from disk */
void BmpFile::readheaders ()
{
	try {
		bmfh.bfType = IdBm ;
		bmfh.bfSize = getBinIO()->read32_le() ;
		bmfh.bfReserved1 = getBinIO()->read16_le() ;
		bmfh.bfReserved2 = getBinIO()->read16_le() ;
		bmfh.bfOffBits = getBinIO()->read32_le() ;
		
		unsigned long tmpSize = getBinIO()->read32_le() ;
		switch (tmpSize) {
			case SizeBMINFOHEADER: {
				// this file is in the Windows bmp format
				subformat = WIN ;
				bmpwin_readheaders () ;
			break ; }

			case SizeBMCOREHEADER: {
				// this file is in the OS/2 bmp format
				subformat = OS2 ;
				bmpos2_readheaders () ;
			break ; }

			default: {
				if (getBinIO()->is_std()) {
					throw NotImplementedError (_("the bmp data from standard input has a format that is not supported (biSize: %lu)."), tmpSize) ;
				}
				else {
					throw NotImplementedError (_("the bmp file \"%s\" has a format that is not supported (biSize: %lu)."), getBinIO()->getName().c_str(), tmpSize) ;
				}
			break ; }
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR: {
				throw SteghideError (_("an error occured while reading the bmp headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::FILE_EOF: {
				throw SteghideError (_("premature end of file \"%s\" while reading bmp headers."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::STDIN_ERR: {
				throw SteghideError (_("an error occured while reading the bmp headers from standard input.")) ;
			break ; }

			case BinaryInputError::STDIN_EOF: {
				throw SteghideError (_("premature end of data from standard input while reading bmp headers.")) ;
			break ; }
		}
	}
}

void BmpFile::bmpwin_readheaders ()
{
	bmih.biSize = SizeBMINFOHEADER ;
	bmih.biWidth = getBinIO()->read32_le () ;
	bmih.biHeight = getBinIO()->read32_le () ;
	bmih.biPlanes = getBinIO()->read16_le () ;
	myassert (bmih.biPlanes == 1) ;
	bmih.biBitCount = getBinIO()->read16_le () ;
	switch (bmih.biBitCount) {
		case 1:
		setSamplesPerVertex (SamplesPerVertex_SmallPalette) ;
		setEmbValueModulus (EmbValueModulus_SmallPalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 4:
		setSamplesPerVertex (SamplesPerVertex_SmallPalette) ;
		setEmbValueModulus (EmbValueModulus_SmallPalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 8:
		setSamplesPerVertex (SamplesPerVertex_LargePalette) ;
		setEmbValueModulus (EmbValueModulus_LargePalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 24:
		setSamplesPerVertex (SamplesPerVertex_RGB) ;
		setEmbValueModulus (EmbValueModulus_RGB) ;
		setRadius (Radius_RGB) ;
		break ;

		default:
			if (getBinIO()->is_std()) {
				throw NotImplementedError (_("the bmp data from standard input has a format that is not supported (biBitCount: %d)."), bmih.biBitCount) ;
			}
			else {
				throw NotImplementedError (_("the bmp file \"%s\" has a format that is not supported (biBitCount: %d)."), getBinIO()->getName().c_str(), bmih.biBitCount) ;
			}
		break ;
	}
	bmih.biCompression = getBinIO()->read32_le () ;
	if (bmih.biCompression != COMPRESSION_BI_RGB) {
		if (getBinIO()->is_std()) {
			throw NotImplementedError (_("the bitmap data from standard input is compressed which is not supported.")) ;
		}
		else {
			throw NotImplementedError (_("the bitmap data in \"%s\" is compressed which is not supported."), getBinIO()->getName().c_str()) ;
		}
	}
	bmih.biSizeImage = getBinIO()->read32_le () ;
	bmih.biXPelsPerMeter = getBinIO()->read32_le () ;
	bmih.biYPelsPerMeter = getBinIO()->read32_le () ;
	bmih.biClrUsed = getBinIO()->read32_le () ;
	bmih.biClrImportant = getBinIO()->read32_le () ;

	if (bmih.biBitCount != 24) {
		/* a color table exists */
		Palette = new ColorPalette() ;
		unsigned int ncolors = 0 ;
		switch (bmih.biBitCount) {
			case 1: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 2 ;
			break ; }

			case 4: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 16 ;
			break ; }

			case 8: {
				ncolors = 256 ;
			break ; }

			default: {
				myassert (0) ;
			break ; }
		}
		if (bmih.biClrUsed != 0) {
			ncolors = bmih.biClrUsed ;
		}

		for (unsigned int i = 0 ; i < ncolors ; i++) {
			unsigned char b = getBinIO()->read8() ;
			unsigned char g = getBinIO()->read8() ;
			unsigned char r = getBinIO()->read8() ;
			if (getBinIO()->read8() != 0) {
				Warning w (_("maybe corrupted windows bmp data (Reserved in RGBQUAD is non-zero).")) ;
				w.printMessage() ;
			}
			Palette->addEntry(r, g, b) ;
		}
	}
}

void BmpFile::bmpos2_readheaders ()
{
	bmch.bcSize = SizeBMCOREHEADER ;
	bmch.bcWidth = getBinIO()->read16_le () ;
	bmch.bcHeight = getBinIO()->read16_le () ;
	bmch.bcPlanes = getBinIO()->read16_le () ;
	myassert (bmch.bcPlanes == 1) ;
	bmch.bcBitCount = getBinIO()->read16_le () ;
	switch (bmch.bcBitCount) {
		case 1:
		setSamplesPerVertex (SamplesPerVertex_SmallPalette) ;
		setEmbValueModulus (EmbValueModulus_SmallPalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 4:
		setSamplesPerVertex (SamplesPerVertex_SmallPalette) ;
		setEmbValueModulus (EmbValueModulus_SmallPalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 8:
		setSamplesPerVertex (SamplesPerVertex_LargePalette) ;
		setEmbValueModulus (EmbValueModulus_LargePalette) ;
		setRadius (Radius_Palette) ;
		break ;

		case 24:
		setSamplesPerVertex (SamplesPerVertex_RGB) ;
		setEmbValueModulus (EmbValueModulus_RGB) ;
		setRadius (Radius_RGB) ;
		break ;

		default:
			if (getBinIO()->is_std()) {
				throw NotImplementedError (_("the bmp data from standard input has a format that is not supported (bcBitCount: %d)."), bmch.bcBitCount) ;
			}
			else {
				throw NotImplementedError (_("the bmp file \"%s\" has a format that is not supported (bcBitCount: %d)."), getBinIO()->getName().c_str(), bmch.bcBitCount) ;
			}
		break ;
	}

	if (bmch.bcBitCount != 24) {
		/* a color table exists */
		unsigned int ncolors = 0 ;
		Palette = new ColorPalette() ;
		switch (bmch.bcBitCount) {
			case 1: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a black/white bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 2 ;
			break ; }

			case 4: {
				if (Args.Command.getValue() == EMBED) {
					Warning w (_("using a 16-color bitmap as cover is very insecure!")) ;
					w.printMessage() ;
				}
				ncolors = 16 ;
			break ; }

			case 8: {
				ncolors = 256 ;
			break ; }

			default: {
				myassert (0) ;
			break ; }
		}

		for (unsigned int i = 0 ; i < ncolors ; i++) {
			unsigned char b = getBinIO()->read8() ;
			unsigned char g = getBinIO()->read8() ;
			unsigned char r = getBinIO()->read8() ;
			Palette->addEntry (r, g, b) ;
		}
	}
}

/* writes the headers of a bmp file to disk */
void BmpFile::writeheaders ()
{
	try {
		getBinIO()->write16_le (bmfh.bfType) ;
		getBinIO()->write32_le (bmfh.bfSize) ;
		getBinIO()->write16_le (bmfh.bfReserved1) ;
		getBinIO()->write16_le (bmfh.bfReserved2) ;
		getBinIO()->write32_le (bmfh.bfOffBits) ;

		switch (getSubformat()) {
			case WIN: {
				bmpwin_writeheaders() ;
			break ; }

			case OS2: {
				bmpos2_writeheaders() ;
			break ; }

			default: {
				myassert (0) ;
			break ; }
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR: {
				throw SteghideError (_("an error occured while writing the bmp headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryOutputError::STDOUT_ERR: {
				throw SteghideError (_("an error occured while writing the bmp headers to standard output.")) ;
			break ; }
		}
	}
}

void BmpFile::bmpwin_writeheaders ()
{
	getBinIO()->write32_le (bmih.biSize) ;
	getBinIO()->write32_le (bmih.biWidth) ;
	getBinIO()->write32_le (bmih.biHeight) ;
	getBinIO()->write16_le (bmih.biPlanes) ;
	getBinIO()->write16_le (bmih.biBitCount) ;
	getBinIO()->write32_le (bmih.biCompression) ;
	getBinIO()->write32_le (bmih.biSizeImage) ;
	getBinIO()->write32_le (bmih.biXPelsPerMeter) ;
	getBinIO()->write32_le (bmih.biYPelsPerMeter) ;
	getBinIO()->write32_le (bmih.biClrUsed) ;
	getBinIO()->write32_le (bmih.biClrImportant) ;

	if (Palette != NULL) {
		for (unsigned int i = 0 ; i < Palette->getSize() ; i++) {
			getBinIO()->write8 ((*Palette)[i].Blue) ;
			getBinIO()->write8 ((*Palette)[i].Green) ;
			getBinIO()->write8 ((*Palette)[i].Red) ;
			getBinIO()->write8 (0) ;
		}
	}
}

void BmpFile::bmpos2_writeheaders ()
{
	getBinIO()->write32_le (bmch.bcSize) ;
	getBinIO()->write16_le (bmch.bcWidth) ;
	getBinIO()->write16_le (bmch.bcHeight) ;
	getBinIO()->write16_le (bmch.bcPlanes) ;
	getBinIO()->write16_le (bmch.bcBitCount) ;

	if (Palette != NULL) {
		for (unsigned int i = 0 ; i < Palette->getSize() ; i++) {
			getBinIO()->write8 ((*Palette)[i].Blue) ;
			getBinIO()->write8 ((*Palette)[i].Green) ;
			getBinIO()->write8 ((*Palette)[i].Red) ;
		}
	}
}

/* returns the number of bytes used to store the pixel data of one scan line */
unsigned long BmpFile::calcLinelength ()
{
	unsigned long retval = 0 ;

	switch (getSubformat()) {
		case WIN: {
			if (bmih.biBitCount * bmih.biWidth % 8 == 0) {
				retval = bmih.biBitCount * bmih.biWidth / 8 ;
			}
			else {
				retval = (bmih.biBitCount * bmih.biWidth / 8) + 1;
			}
		break ; }

		case OS2: {
			if (bmch.bcBitCount * bmch.bcWidth % 8 == 0) {
				retval = bmch.bcBitCount * bmch.bcWidth / 8 ;
			}
			else {
				retval = (bmch.bcBitCount * bmch.bcWidth / 8) + 1;
			}
		break ; }
		
		default: {
			myassert (0) ;
		break ; }
	}

	return retval ;
}


/* reads a bmp file from disk into a CVRSTGFILE structure */
void BmpFile::readdata ()
{
	try {
		unsigned long linelength = calcLinelength () ;
		unsigned long height = getHeight () ;

		int paddinglength = 0 ;
		if (linelength % 4 == 0) {
			paddinglength = 0 ;
		}
		else {
			paddinglength = 4 - (linelength % 4) ;
		}

		BitmapData.resize (height * linelength) ;
		for (unsigned long line = 0 ; line < height ; line++) {
			for (unsigned long posinline = 0 ; posinline < linelength ; posinline++) {
				BitmapData[line * linelength + posinline] = getBinIO()->read8() ;
			}

			for (int i = 0 ; i < paddinglength ; i++) {
				if (getBinIO()->read8() != 0) {
					Warning w (_("maybe corrupted bmp data (padding byte at 0x%lx set to non-zero)."), getBinIO()->getPos() - 1) ;
					w.printMessage() ;
				}
			}
		}

		atend.clear() ;
		while (!getBinIO()->eof()) {
			atend.push_back (getBinIO()->read8()) ;
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR: {
				throw SteghideError (_("an error occured while reading the bmp data from the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::FILE_EOF: {
				throw SteghideError (_("premature end of file \"%s\" while reading bmp data."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryInputError::STDIN_ERR: {
				throw SteghideError (_("an error occured while reading the bmp data from standard input.")) ;
			break ; }

			case BinaryInputError::STDIN_EOF: {
				throw SteghideError (_("premature end of bmp data from standard input.")) ;
			break ; }
		}
	}
}

void BmpFile::writedata ()
{
	try {
		unsigned long linelength = calcLinelength () ;
		unsigned long height = getHeight () ;

		unsigned int paddinglength = 0 ;
		if (linelength % 4 == 0) {
			paddinglength = 0 ;
		}
		else {
			paddinglength = 4 - (linelength % 4) ;
		}

		for (unsigned long line = 0 ; line < height ; line++) {
			for (unsigned long posinline = 0 ; posinline < linelength ; posinline++) {
				getBinIO()->write8 (BitmapData[line * linelength + posinline]) ;
			}

			// write padding bytes
			for (unsigned int i = 0 ; i < paddinglength ; i++) {
				getBinIO()->write8 (0) ;
			}
		}

		for (std::vector<unsigned char>::iterator i = atend.begin() ; i != atend.end() ; i++) {
			getBinIO()->write8 (*i) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR: {
				throw SteghideError (_("an error occured while writing the bitmap data to the file \"%s\"."), getBinIO()->getName().c_str()) ;
			break ; }

			case BinaryOutputError::STDOUT_ERR: {
				throw SteghideError (_("an error occured while writing the bitmap data to standard output.")) ;
			break ; }
		}
	}
}

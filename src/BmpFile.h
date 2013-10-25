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

#ifndef SH_BMPFILE_H
#define SH_BMPFILE_H

#include <vector>

#include "CvrStgFile.h"

class BinaryIO ;
class ColorPalette ;

class BmpFile : public CvrStgFile {
	public:
	BmpFile (void) ;
	BmpFile (BinaryIO *io) ;
	~BmpFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	std::list<CvrStgFile::Property> getProperties (void) const ;
	std::vector<MatchingAlgorithm*> getMatchingAlgorithms (Graph* g, Matching* m) const ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (SamplePos pos) const ;

	std::vector<SampleValueAdjacencyList*> calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const ;

	unsigned short getBitCount (void) const ;
	unsigned long getWidth (void) const ;
	unsigned long getHeight (void) const ;
	ColorPalette *getPalette (void) const ;

	protected:
	typedef struct struct_BITMAPFILEHEADER {
		unsigned short	bfType ;
		unsigned long	bfSize ;
		unsigned short	bfReserved1 ;
		unsigned short	bfReserved2 ;
		unsigned long	bfOffBits ;
	} BITMAPFILEHEADER ;

	/* windows bmp file format */
	typedef struct struct_BITMAPINFOHEADER {
		unsigned long	biSize ;
		signed long		biWidth ;
		signed long		biHeight ;
		unsigned short	biPlanes ;
		unsigned short	biBitCount ;
		unsigned long	biCompression ;
		unsigned long	biSizeImage ;
		signed long		biXPelsPerMeter ;
		signed long		biYPelsPerMeter ;
		unsigned long	biClrUsed ;
		unsigned long	biClrImportant ;
	} BITMAPINFOHEADER ;

	/* os/2 bmp file format */
	typedef struct struct_BITMAPCOREHEADER {
		unsigned long	bcSize;
		unsigned short	bcWidth;
		unsigned short	bcHeight;
		unsigned short	bcPlanes;
		unsigned short	bcBitCount;
	} BITMAPCOREHEADER ;

	private:
	static const unsigned int IdBm = 19778 ;
	static const unsigned short SizeBMFILEHEADER = 14 ;
	static const unsigned short SizeBMINFOHEADER = 40 ;
	static const unsigned short SizeBMCOREHEADER = 12 ;
	static const unsigned int COMPRESSION_BI_RGB = 0 ;

	static const unsigned short SamplesPerVertex_SmallPalette = 2 ;
	static const unsigned short SamplesPerVertex_LargePalette = 3 ;
	static const unsigned short SamplesPerVertex_RGB = 2 ;
	/// the default radius for palette images (400 = 20^2)
	static const UWORD32 Radius_Palette = 400 ;
	/// the default radius for RGB images (100 = 10^2)
	static const UWORD32 Radius_RGB = 100 ;

	static const EmbValue EmbValueModulus_SmallPalette = 2 ;
	static const EmbValue EmbValueModulus_LargePalette = 4 ;
	static const EmbValue EmbValueModulus_RGB = 4 ; // if changed here - also change BmpRGBSampleValue::calcEValue

	enum SUBFORMAT { WIN, OS2 } ;

	SUBFORMAT subformat ;
	BITMAPFILEHEADER bmfh ;
	BITMAPINFOHEADER bmih ;
	BITMAPCOREHEADER bmch ;
	ColorPalette* Palette ;
	/**
	 * contains the bitmap in the following format
	 * bitmap[i] is the pixel data of the i-th row of the bitmap
	 * bitmap[i][j] is the j-th byte of the pixel data of the i-th row of the bitmap
	 * if bitcount is < 8 then bitmap[i][j] contains the pixels as read in from the file (i.e. in the "wrong" direction)
	 * this is taken care of in the calcRCB function
	 **/
	std::vector<std::vector <unsigned char> > bitmap ;

	/**
	 * contains the bitmap data in the same order as read from file (but without padding bytes)
	 **/
	std::vector<BYTE> BitmapData ;

	/// contains bytes that are appended at the end of the bitmap data (some image editors apparently do this)
	std::vector<BYTE> atend ;

	void readheaders () ;
	void bmpwin_readheaders () ;
	void bmpos2_readheaders () ;
	void writeheaders () ;
	void bmpwin_writeheaders () ;
	void bmpos2_writeheaders () ;
	void readdata () ;
	void writedata () ;
	/**
	 * translate a sample position into a <index,firstbit> pair "pointing" into the BitmapData array
	 * \param pos a sample position
	 * \param index a pointer to a variable that will contain the array index used to access the pos-th sample
	 * \param firstbit the firstbit in BitmapData[index] that belongs to the sample with the given position
	 **/
	void calcIndex (SamplePos pos, unsigned long* index, unsigned short* firstbit) const ;
	unsigned long calcLinelength () ;
	SUBFORMAT getSubformat (void) const ;
} ;

#endif /* ndef SH_BMPFILE_H */

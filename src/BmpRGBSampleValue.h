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

#ifndef SH_BMPRGBSAMPLEVALUE_H
#define SH_BMPRGBSAMPLEVALUE_H

#include "BmpSampleValue.h"
#include "RGBTriple.h"
#include "common.h"

/**
 * \class BmpRGBSampleValue
 * \brief a sample in a bmp rgb (i.e. 24-bit) file
 **/
class BmpRGBSampleValue : public BmpSampleValue {
	public:
	BmpRGBSampleValue (BYTE r, BYTE g, BYTE b) ;
	BmpRGBSampleValue (RGBTriple t) ;

	UWORD32 calcDistance (const SampleValue *s) const ;
	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;
	std::string getName (void) const ;

	BYTE getRed (void) const { return Color.Red ; } ;
	BYTE getGreen (void) const { return Color.Green ; } ;
	BYTE getBlue (void) const { return Color.Blue ; } ;

	private:
	RGBTriple Color ;

	UWORD32 calcKey (const RGBTriple& rgb) const
		{ return (((UWORD32) rgb.Red << 16) | ((UWORD32) rgb.Green << 8) | ((UWORD32) rgb.Blue)) ;} ;

	EmbValue calcEValue (const RGBTriple& rgb) const
		// { /* for Modulus 8 */ return ((EmbValue) (((rgb.Red & 1) << 2) | ((rgb.Green & 1) << 1) | (rgb.Blue & 1))) ; } ;
		{ /* for Modulus 4 */ return ((EmbValue) ((((rgb.Red & 1) ^ (rgb.Green & 1)) << 1) | ((rgb.Red & 1) ^ (rgb.Blue & 1)))) ; } ;

	enum COLOR { RED, GREEN, BLUE } ;
	enum DIRECTION { UP, DOWN } ;

	/**
	 * add the BYTEs a and b
	 * \return min(255, a + b)
	 **/
	BYTE plus (BYTE a, BYTE b) const ;

	/**
	 * substract the BYTE b from the BYTE a
	 * \return max(0, a - b)
	 **/
	BYTE minus (BYTE a, BYTE b) const ;

	/**
	 * add candidates for the nearest target sample value
	 * \param cands the candidates vector
	 * \param cube the color values describing the current search cube
	 * \param fc the fixed color
	 * \param fd the fixed side of the fixed color
	 **/
	void addNTSVCandidates (std::vector<RGBTriple>& cands, const BYTE cube[3][2], COLOR fc, DIRECTION fd, COLOR i1, COLOR i2, EmbValue t) const ;
} ;

#endif // ndef SH_BMPRGBSAMPLEVALUE_H

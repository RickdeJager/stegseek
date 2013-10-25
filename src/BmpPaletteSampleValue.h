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

#ifndef SH_BMPPALETTESAMPLEVALUE_H
#define SH_BMPPALETTESAMPLEVALUE_H

#include "BmpSampleValue.h"
#include "CvrStgFile.h"
#include "ColorPalette.h"
#include "common.h"

/**
 * \class BmpPaletteSampleValue
 * \brief a sample in a bmp palette (i.e. in a 1-,4- or 8-bit) file
 **/
class BmpPaletteSampleValue : public BmpSampleValue {
	public:
	BmpPaletteSampleValue (BYTE i) ;

	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;
	std::string getName (void) const ;

	BYTE getIndex (void) const { return Index ; } ;
	BYTE getRed (void) const { return (*Palette)[Index].Red ; } ;
	BYTE getGreen (void) const { return (*Palette)[Index].Green ; } ;
	BYTE getBlue (void) const { return (*Palette)[Index].Blue ; } ;

	private:
	ColorPalette* Palette ;
	BYTE Index ;

	EmbValue calcEValue (BYTE idx) const
		{ return (idx % Globs.TheCvrStgFile->getEmbValueModulus()) ; } ;
} ;

#endif // ndef SH_BMPPALETTESAMPLEVALUE_H

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

#include "BmpFile.h"
#include "BmpPaletteSampleValue.h"
#include "ColorPalette.h"

BmpPaletteSampleValue::BmpPaletteSampleValue (unsigned char i)
	: BmpSampleValue(), Index(i)
{
	const BmpFile* bmpfile = dynamic_cast<const BmpFile*> (Globs.TheCvrStgFile) ;
	myassert (bmpfile) ;
	Palette = bmpfile->getPalette() ;
	Key = ((unsigned long) getIndex() << 24) | ((unsigned long) getRed() << 16) |
		  ((unsigned long) getGreen() << 8) | ((unsigned long) getBlue()) ;
	EValue = calcEValue(getIndex()) ;
}

SampleValue* BmpPaletteSampleValue::getNearestTargetSampleValue (EmbValue t) const
{
	BmpPaletteSampleValue* sv_mindist = NULL ;
	UWORD32 mindist = UWORD32_MAX ;
	for (unsigned int i = 0 ; i < Palette->getSize() ; i++) {
		if (calcEValue(i) == t) {
			BmpPaletteSampleValue* destsv = new BmpPaletteSampleValue (i) ;
			UWORD32 curdist = calcDistance (destsv) ;
			if (curdist < mindist) {
				delete sv_mindist ;
				sv_mindist = destsv ;
				mindist = curdist ;
			}
			else {
				delete destsv ;
			}
		}
	}
	myassert (sv_mindist != NULL) ;
	return ((SampleValue*) sv_mindist) ;
}

std::string BmpPaletteSampleValue::getName () const
{
	char buf[128] ;
	sprintf (buf, "i%ur%ug%ub%u", getIndex(), getRed(), getGreen(), getBlue()) ;
	return std::string (buf) ;
}

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

#include "BmpRGBSampleValue.h"

BmpRGBSampleValue::BmpRGBSampleValue (BYTE r, BYTE g, BYTE b)
	: BmpSampleValue(), Color(r, g, b)
{
	Key = calcKey (Color) ;
	EValue = calcEValue (Color) ;
}

BmpRGBSampleValue::BmpRGBSampleValue (RGBTriple t)
	: BmpSampleValue(), Color(t)
{
	Key = calcKey (Color) ;
	EValue = calcEValue (Color) ;
}

UWORD32 BmpRGBSampleValue::calcDistance (const SampleValue *s) const
{
	const BmpRGBSampleValue *sample = (const BmpRGBSampleValue*) s ;
	/* If s is not a BmpRGBSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a BmpRGBSampleValue as s anyway. */

	int dr = (int) Color.Red - (int) sample->Color.Red ;
	int dg = (int) Color.Green - (int) sample->Color.Green ;
	int db = (int) Color.Blue - (int) sample->Color.Blue ;
	return (UWORD32) (dr*dr + dg*dg + db*db) ;
}

SampleValue *BmpRGBSampleValue::getNearestTargetSampleValue (EmbValue t) const
{
	std::vector<RGBTriple> candidates ;

	BYTE cube[3][2] ;
	cube[RED][UP] = Color.Red ; cube[RED][DOWN] = Color.Red ;
	cube[GREEN][UP] = Color.Green ; cube[GREEN][DOWN] = Color.Green ;
	cube[BLUE][UP] = Color.Blue ; cube[BLUE][DOWN] = Color.Blue ;

	do {
		cube[RED][UP] = plus(cube[RED][UP], 1) ;
		cube[RED][DOWN] = minus(cube[RED][DOWN], 1) ;
		cube[GREEN][UP] = plus(cube[GREEN][UP], 1) ;
		cube[GREEN][DOWN] = minus(cube[GREEN][DOWN], 1) ;
		cube[BLUE][UP] = plus(cube[BLUE][UP], 1) ;
		cube[BLUE][DOWN] = minus(cube[BLUE][DOWN], 1) ;

		addNTSVCandidates (candidates, cube, RED, UP, GREEN, BLUE, t) ;
		addNTSVCandidates (candidates, cube, RED, DOWN, GREEN, BLUE, t) ;
		addNTSVCandidates (candidates, cube, GREEN, UP, RED, BLUE, t) ;
		addNTSVCandidates (candidates, cube, GREEN, DOWN, RED, BLUE, t) ;
		addNTSVCandidates (candidates, cube, BLUE, UP, RED, GREEN, t) ;
		addNTSVCandidates (candidates, cube, BLUE, DOWN, RED, GREEN, t) ;
	} while (candidates.empty()) ;

	// calculate minimal distance
	UWORD32 mindist = UWORD32_MAX ;
	for (std::vector<RGBTriple>::const_iterator cit = candidates.begin() ; cit != candidates.end() ; cit++) {
		UWORD32 curdist = Color.calcDistance(*cit) ;
		if (curdist < mindist) {
			mindist = curdist ;
		}
	}

	// choose only from those with the minimal distance
	std::vector<RGBTriple> finalcandidates ;
	for (std::vector<RGBTriple>::const_iterator cit = candidates.begin() ; cit != candidates.end() ; cit++) {
		if (Color.calcDistance(*cit) == mindist) {
			finalcandidates.push_back(*cit) ;
		}
	}

	unsigned int rnd = (unsigned int) RndSrc.getValue (finalcandidates.size()) ;
	return ((SampleValue*) new BmpRGBSampleValue (finalcandidates[rnd])) ;
}

void BmpRGBSampleValue::addNTSVCandidates (std::vector<RGBTriple>& cands, const BYTE cube[3][2], COLOR fc, DIRECTION fd, COLOR i1, COLOR i2, EmbValue t) const
{
	for (BYTE value1 = cube[i1][DOWN] ; value1 < cube[i1][UP] ; value1++) {
		for (BYTE value2 = cube[i2][DOWN] ; value2 < cube[i2][UP] ; value2++) {
			// create an RGBTriple
			BYTE color[3] ;
			color[fc] = cube[fc][fd] ;
			color[i1] = value1 ;
			color[i2] = value2 ;
			RGBTriple rgb (color[RED], color[GREEN], color[BLUE]) ;
			if (calcEValue (rgb) == t) {
				// add rgb to candidates...
				bool found = false ;
				for (std::vector<RGBTriple>::const_iterator cit = cands.begin() ; cit != cands.end() ; cit++) {
					if (*cit == rgb) {
						found = true ;
					}
				}
				if (!found) {
					cands.push_back (rgb) ;
				}
			}
		}
	}
}

std::string BmpRGBSampleValue::getName () const
{
	char buf[128] ;
	sprintf (buf, "r%ug%ub%u", getRed(), getGreen(), getBlue()) ;
	return std::string (buf) ;
}

BYTE BmpRGBSampleValue::minus (BYTE a, BYTE b) const
{
	BYTE retval = 0 ;
	if (a > b) {
		retval = a - b ;
	}
	return retval ;
}

BYTE BmpRGBSampleValue::plus (BYTE a, BYTE b) const
{
	unsigned int sum = a + b ;
	return ((sum <= 255) ? sum : 255) ;
}

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

#include <cstdlib>
#include <cmath>

#include "common.h"
#include "JpegSampleValue.h"

JpegSampleValue::JpegSampleValue (int c)
	: SampleValue(), DctCoeff (c)
{
	Key = (UWORD32) DctCoeff ;
	EValue = calcEValue (DctCoeff) ;
}

SampleValue *JpegSampleValue::getNearestTargetSampleValue (EmbValue t) const
{
	SWORD16 minvalue = 0, maxvalue = 0 ;

	if (DctCoeff > 0) {
		minvalue = 1 ;
		maxvalue = SWORD16_MAX ;
	}
	else if (DctCoeff < 0) {
		minvalue = SWORD16_MIN ;
		maxvalue = -1 ;
	}
	else {
		myassert(false) ;
	}

	SWORD16 dctc_up = DctCoeff, dctc_down = DctCoeff, dctc_new = 0 ;
	bool found = false ;
	do {
		if (dctc_up < maxvalue) {
			dctc_up++ ;
		}
		if (dctc_down > minvalue) {
			dctc_down-- ;
		}

		if (calcEValue(dctc_up) == t && calcEValue(dctc_down) == t) {
			if (RndSrc.getBool()) {
				dctc_new = dctc_up ;
			}
			else {
				dctc_new = dctc_down ;
			}
			found = true ;
		}
		else if (calcEValue(dctc_up) == t) {
			dctc_new = dctc_up ;
			found = true ;
		}
		else if (calcEValue(dctc_down) == t) {
			dctc_new = dctc_down ;
			found = true ;
		}
	} while (!found) ;

	return ((SampleValue *) new JpegSampleValue (dctc_new)) ;
}

UWORD32 JpegSampleValue::calcDistance (const SampleValue *s) const
{
	const JpegSampleValue *sample = (const JpegSampleValue*) s ;
	/* If s is not a JpegSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a JpegSampleValue as s anyway. */

	int d = DctCoeff - sample->DctCoeff ;
	return ((d >= 0) ? ((UWORD32) d) : ((UWORD32) -d)) ;
}

std::string JpegSampleValue::getName (void) const
{
	char buf[128] ;
	sprintf (buf, "%d", DctCoeff) ;
	return std::string (buf) ;
}

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

#include <cmath>
#include <cstdlib>

#include "common.h"
#include "WavFile.h"
#include "WavPCMSampleValue.h"

WavPCMSampleValue::WavPCMSampleValue (int v)
	: SampleValue(), Value(v)
{
	const WavFile* wavfile = dynamic_cast<const WavFile*> (Globs.TheCvrStgFile) ;
	myassert (wavfile != NULL) ;
	unsigned short samplesize = wavfile->getBitsPerSample() ;

	int maxvalue = 1 ;
	int minvalue = 1 ;
	if ((samplesize >= 1) && (samplesize <= 8)) {
		// maxvalue is 2^samplesize
		for (unsigned short i = 0 ; i < samplesize ; i++) {
			maxvalue *= 2 ;
		}

		minvalue = 0 ;
	}
	else {
		// maxvalue is (2^(samplesize - 1)) - 1
		for (unsigned short i = 0 ; i < (samplesize - 1) ; i++) {
			maxvalue *= 2 ;
		}
		maxvalue-- ;

		// minvalue is -(2^(SampleSize - 1))
		for (unsigned short i = 0 ; i < (samplesize - 1) ; i++) {
			minvalue *= 2 ;
		}
		minvalue = -minvalue ;
	}

	MaxValue = maxvalue ;
	MinValue = minvalue ;
	if (MinValue > Value) {
		std::cerr << "MinValue: " << MinValue << ", Value: " << Value << std::endl ;
	}
	myassert (MinValue <= Value) ;
	myassert (Value <= MaxValue) ;

	Key = (UWORD32) Value ;
	EValue = calcEValue (Value) ;
}

SampleValue *WavPCMSampleValue::getNearestTargetSampleValue (EmbValue t) const
{
	int val_up = Value, val_down = Value, newval = 0 ;
	bool found = false ;

	do {
		if (val_up < MaxValue) {
			val_up++ ;
		}
		if (val_down > MinValue) {
			val_down-- ;
		}

		if (calcEValue(val_up) == t && calcEValue(val_down) == t) {
			if (RndSrc.getBool()) {
				newval = val_up ;
			}
			else {
				newval = val_down ;
			}
			found = true ;
		}
		else if (calcEValue(val_up) == t) {
			newval = val_up ;
			found = true ;
		}
		else if (calcEValue(val_down) == t) {
			newval = val_down ;
			found = true ;
		}
	} while (!found) ;

	return ((SampleValue *) new WavPCMSampleValue (newval)) ;
}

UWORD32 WavPCMSampleValue::calcDistance (const SampleValue *s) const
{
	const WavPCMSampleValue *sample = (const WavPCMSampleValue*) s ;
	/* If s is not a WavPCMSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a WavPCMSampleValue as s anyway. */

	int d = Value - sample->Value ;
	return ((d >= 0) ? ((UWORD32) d) : ((UWORD32) -d)) ;
}

std::string WavPCMSampleValue::getName () const
{
	char buf[128] ;
	sprintf (buf, "%d", Value) ;
	return std::string (buf) ;
}

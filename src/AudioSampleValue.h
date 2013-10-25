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

#ifndef SH_AUDIOSAMPLEVALUE_H
#define SH_AUDIOSAMPLEVALUE_H

#include "CvrStgFile.h"
#include "SampleValue.h"
#include "common.h"

/**
 * An enumeration containing all audio sample types. This enumeration
 * is used as template parameter for AudioSampleValue and AudioData to
 * identify the correct implementation for a given audio format.
 **/
enum AUDIOSAMPLETYPE {
	/// au 8 Bit mu-law
	AuMuLaw,
	/// au 8 Bit linear pcm
	AuPCM8,
	/// au 16 Bit linear pcm
	AuPCM16,
	/// au 32 Bit linear pcm
	AuPCM32
} ;

/**
 * \class AudioSampleValue
 * \brief a class representing an audio sample
 **/
template<AUDIOSAMPLETYPE Type, class ValueType>
class AudioSampleValue : public SampleValue {
	public:
	AudioSampleValue (ValueType v) ;

	ValueType getValue (void) const { return Value ; } ;

	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;
	UWORD32 calcDistance (const SampleValue* s) const ;
	std::string getName (void) const ;

	private:
	ValueType Value ;
	static const ValueType MinValue ;
	static const ValueType MaxValue ;

	UWORD32 calcKey (ValueType v) const { return (v - MinValue) ; } ;
	EmbValue calcEValue (ValueType v) const { return ((EmbValue) ((v - MinValue) % Globs.TheCvrStgFile->getEmbValueModulus())) ; } ;
} ;

template<AUDIOSAMPLETYPE Type, class ValueType>
AudioSampleValue<Type,ValueType>::AudioSampleValue (ValueType v)
	: SampleValue(), Value(v)
{
	Key = calcKey(v) ;
	EValue = calcEValue(v) ;
}

template<AUDIOSAMPLETYPE Type, class ValueType>
UWORD32 AudioSampleValue<Type,ValueType>::calcDistance (const SampleValue* s) const
{
	const AudioSampleValue<Type,ValueType>* sample = (const AudioSampleValue<Type,ValueType>*) s ;
	/* If s is not a correct AudioSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a correct AudioSampleValue as s anyway. */

	if (sample->Value > Value) {
		return sample->Value - Value ;
	}
	else {
		return Value - sample->Value ;
	}
}

template<AUDIOSAMPLETYPE Type, class ValueType>
SampleValue* AudioSampleValue<Type,ValueType>::getNearestTargetSampleValue (EmbValue t) const
{
	ValueType val_up = Value, val_down = Value, newval = 0 ;
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

	return ((SampleValue *) new AudioSampleValue<Type,ValueType> (newval)) ;
}

template<AUDIOSAMPLETYPE Type, class ValueType>
std::string AudioSampleValue<Type,ValueType>::getName (void) const
{
	char buf[128] ;
	sprintf (buf, "%ld", (long) Value) ;
	return std::string (buf) ;
}

#endif // ndef SH_AUDIOSAMPLEVALUE_H

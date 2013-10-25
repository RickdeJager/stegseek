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

#ifndef SH_WAVSAMPLE_H
#define SH_WAVSAMPLE_H

#include <functional>

#include "SampleValue.h"

class CvrStgFile ;

class WavPCMSampleValue : public SampleValue {
	public:
	WavPCMSampleValue (int v) ;

	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;
	UWORD32 calcDistance (const SampleValue *s) const ;
	std::string getName (void) const ;

	int getValue (void) const
		{ return Value ; } ;

	private:
	int Value ;
	int MaxValue ;
	int MinValue ;

	EmbValue calcEValue (int v) const
		{ return (v & 1) ; }
} ;

class WavPCMSmaller : public std::binary_function<WavPCMSampleValue*,WavPCMSampleValue*,bool> {
	public:
	bool operator() (const WavPCMSampleValue *v, const WavPCMSampleValue *w) { return v->getValue() < w->getValue() ; } ;
} ;

#endif // ndef SH_WAVSAMPLE_H

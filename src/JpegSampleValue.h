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

#ifndef SH_JPEGSAMPLE_H
#define SH_JPEGSAMPLE_H

#include "SampleValue.h"

class JpegSampleValue : public SampleValue {
	public:
	JpegSampleValue (int c) ;

	SampleValue* getNearestTargetSampleValue (EmbValue t) const ;
	UWORD32 calcDistance (const SampleValue *s) const ;
	std::string getName (void) const ;

	SWORD16 getDctCoeff (void) const
		{ return DctCoeff ; } ;

	static EmbValue calcEValue (SWORD16 dctc)
		{ return (((dctc >= 0) ? dctc : -dctc)  % 2) ; } ;

	private:
	SWORD16 DctCoeff ;
} ;

#endif // ndef SH_JPEGSAMPLE_H

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

#include "CvrStgFile.h"

#include "DummyFile.h"
#include "DummySampleValue.h"

UWORD32 DummySampleValue::calcDistance (const SampleValue* s) const
{
	DummySampleValue *sample = (DummySampleValue*) s ;
	return ((sample->Value > Value) ? (sample->Value - Value) : (Value - sample->Value)) ;
}

bool DummySampleValue::isNeighbour (const SampleValue* s) const
{
	DummyFile *f = (DummyFile*) Globs.TheCvrStgFile ;
	DummySampleValue *sample = (DummySampleValue*) s ;
	std::vector<std::vector<bool> >* svam = f->getSampleValueAdjacencyMatrix() ;
	return (*svam)[Value][sample->getValue()] ;
}

SampleValue* DummySampleValue::getNearestTargetSampleValue (EmbValue t) const
{
	UWORD16 n_value = 0 ;
	if (Value == 0) {
		n_value = 1 ;
	}
	else if (Value == UWORD16_MAX) {
		n_value = UWORD16_MAX - 1 ;
	}
	else {
		if (RndSrc.getBool()) {
			n_value = Value - 1 ;
		}
		else {
			n_value = Value + 1 ;
		}
	}
	myassert (n_value % 2 == t) ;
	return ((SampleValue *) new DummySampleValue (n_value)) ;
}

std::string DummySampleValue::getName (void) const
{
	char buf[128] ;
	sprintf (buf, "%d", Value) ;
	return std::string (buf) ;
}

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

#ifndef SH_CVRSTGOBJECT_H
#define SH_CVRSTGOBJECT_H

#include "common.h"

class SampleValue ;

/**
 * \class CvrStgObject
 * \brief an object that can hold embedded data
 *
 * This abstract base class provides an interface for every class that is able
 * to hold embedded data. Something that can hold embedded data is essentially
 * though of as an array of samples.
 *
 * Definitions:
 * Embedded Bit...a bit to be embedded (one bit in the original or extracted embfile)
 * Sample...the smallest data unit in a file (e.g. a RGB triple, a DCT coefficient)
 **/
class CvrStgObject {
	public:
	/**
	 * get the number of samples in this CvrStgObject
	 **/
	virtual unsigned long getNumSamples (void) const = 0 ;

	/**
	 * get the sample at position pos
	 * \param pos the position of a sample (must be in 0...getNumSamples()-1)
	 * \return the sample at the given position
	 *
	 * The sample object is created in this function and should be deleted by the caller.
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	virtual SampleValue* getSampleValue (const SamplePos pos) const = 0 ;

	/**
	 * replace a sample thus (possibly) altering the value of the bit returned by SampleValue->getBit()
	 * \param pos the position of the sample (must be in 0...getNumSamples()-1)
	 * \param s the sample value that should replace the current sample value (must be of correct type for this CvrStgObject)
	 *
	 * The derived class should check the condition(s) given above in its Implementation of this function.
	 **/
	virtual void replaceSample (const SamplePos pos, const SampleValue* s) = 0 ;
} ;

#endif //ndef SH_CVRSTGOBJECT_H

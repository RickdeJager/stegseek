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

#ifndef SH_SAMPLEVALUE_H
#define SH_SAMPLEVALUE_H

#include <functional>

#include "common.h"
#include "wrapper_hash_set.h"

class CvrStgFile ;

/**
 * \class SampleValue
 * \brief the value of a sample in a CvrStgFile
 *
 * This is the abstract base class for all AuSampleValue, BmpSampleValue, etc. classes
 *
 * For two sample values s1 and s2:
 *
 * s1->calcDistance(s2) == s2->calcDistance(s1) is always true.
 *
 * s1->isNeighbour(s2) == s2->isNeighbour(s1) is always true.
 *
 * s1 and s2 are called opposite if s1->getBit() != s2->getBit()
 *
 * s1 and s2 are called neighbours if s1->isNeighbour(s2) is true
 *
 * s1->getKey() == s2->getKey() iff s1 == s2 
 *
 * s1 == s2 implies s1->getDistance(s2) == 0
 * BUT: s1->getDistance(s2) == 0 does not imply s1 == s2
 *      example: 8-bit bmp palette image - same color value for two different indices
 *
 * s1 == s2 implies s1->getBit() == s2->getBit()
 *
 * s1->getDistance(s2) == 0 implies s1->getBit() == s2->getBit()
 *
 * <b>NOTE:</b> SampleValue and all derived classes rely on the Globals object
 * pointed to by the Globs pointer. This means that it must be set correctly
 * before using any method of a SampleValue (or derived) object.
 **/
class SampleValue {
	public:
	SampleValue (void) ;

	virtual ~SampleValue (void) ;

	/**
	 * get the nearest (with the least distance to this sample value) sample value whose
	 * embedded value equals the specified target
	 * \param t the target embedded value
	 *
	 * If two or more target sample values have equal distance each of them should be
	 * returned with equal probability.
	 *
	 * The returned SampleValue object should be deleted by the callser.
	 **/
	virtual SampleValue* getNearestTargetSampleValue (EmbValue t) const = 0 ;

	/**
	 * calculate the distance between the sample value s and this sample value
	 * \param s a sample value of the same type as this
	 * \return the distance
	 **/
	virtual UWORD32 calcDistance (const SampleValue *s) const = 0 ;

	/**
	 * return a short name uniquely identifying this sample value
	 **/
	virtual std::string getName (void) const = 0 ;

	/**
	 * is the sample value s a neighbour of this sample value ?
	 * \return true iff this and s are neighbours
	 *
	 * This is implemented as (calcDistance() <= Radius) but may be overridden by
	 * derived classes.
	 **/
	virtual bool isNeighbour (const SampleValue* s) const ;

	/**
	 * get the value that is embedded in this sample value (must be >=0 and <EmbValueModulus)
	 * \return the embedded value
	 **/
	EmbValue getEmbeddedValue (void) const
		{ return EValue ; } ;

	/**
	 * get the key for this sample
	 * \return a key which must be different for two different samples values.
	 **/
	UWORD32 getKey (void) const
		{ return Key ; } ;

	/**
	 * two sample values are equal iff their keys are equal
	 **/
	bool operator== (const SampleValue& sv) const { return (getKey() == sv.getKey()) ; } ;
	bool operator!= (const SampleValue& sv) const { return (getKey() != sv.getKey()) ; } ;

	bool operator< (const SampleValue& sv) const { return (getKey() < sv.getKey()) ; } ;

	UWORD32 getNumEdges (EmbValue t) const { return NumEdges[t] ; } ;
	void setNumEdges (EmbValue t, UWORD32 ne) { NumEdges[t] = ne ; } ;
	void incNumEdges (EmbValue t) ;
	void decNumEdges (EmbValue t) ;

	void setLabel (unsigned long l) { Label = l ; } ;
	unsigned long getLabel (void) const { return Label ; } ;

	void print (unsigned short spc = 0) const ;

	protected:
	/// the bit that is embedded in this sample value - must be set in constructor of derived class
	EmbValue EValue ;

	/// the key of this sample value - must be different for two different sample values - must be set in constructor of derived class
	UWORD32 Key ;

	private:
	unsigned long Label ;

	/**
	 * NumEdges[t] contains the number of edges that are added to a vertex if
	 * this sample value with corresponding target value t is added to the vertex
	 **/
	UWORD32* NumEdges ;
} ;

struct SampleValuesEqual : public std::binary_function<SampleValue*, SampleValue*, bool> {
	bool operator() (const SampleValue* s1, const SampleValue *s2) const
	{
		return (*s1 == *s2) ;
	}
} ;

struct SampleValuesLess : public std::binary_function<SampleValue*, SampleValue*, bool> {
	bool operator() (const SampleValue* s1, const SampleValue *s2) const
	{
		return (*s1 < *s2) ;
	}
} ;

struct SampleValueHash : public std::unary_function<SampleValue*,size_t> {
	size_t operator() (const SampleValue* s) const
	{
		sgi::hash<UWORD32> h ;
		return h(s->getKey()) ;
	}
} ;

#endif // ndef SH_CVRSTGSAMPLE_H

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

#ifndef SH_CVRSTGFILE_H
#define SH_CVRSTGFILE_H

#include <list>
#include <string>
#include <map>

#include "BinaryIO.h"
#include "CvrStgObject.h"
class Graph ;
class Matching ;
class MatchingAlgorithm ;
class SampleValue ;
class SampleValueAdjacencyList ;

/**
 * \class CvrStgFile
 * \brief a cover-/stego-file
 *
 * file-format specific constants are handled as follows:
 * CvrStgFile contains a protected set-function (e.g. setSamplesPerVertex), a public
 * get-function (e.g. getSamplesPerVertex() const) and a private variable. The public get
 * function does nothing else than returning the private variable, which must be set
 * as soon as possible (if it is not set, it will contain a null value set in CvrStgFile::CvrStgFile).
 **/
class CvrStgFile : public CvrStgObject {
	public:
	/**
	 * this function reads the file with name fn and returns
	 * a *File object of the correct type casted to CvrStgFile.
	 **/
	static CvrStgFile *readFile (const std::string& fn) ;

	class Property : private std::pair<std::string,std::string> {
		public:
		Property (std::string key, std::string value)
			: std::pair<std::string,std::string> (key, value) {} ;

		std::string getKey (void) const
			{ return first ; } ;

		std::string getValue (void) const
			{ return second ; } ;
	} ;

	CvrStgFile (void) ;
	virtual ~CvrStgFile (void) ;

	virtual void read (BinaryIO *io) ;
	virtual void write (void) ;
	void transform (const std::string& fn) ;

	virtual std::list<Property> getProperties (void) const = 0 ;

	/**
	 * calculate a vector a SampleValueAdjacencyLists
	 * \param svs a vector of unique(!) sample values where svs[i]->getLabel() == i holds for all i
	 * \return a vector of SampleValueAdjacencyLists where retval[i] only contains sample values with getEmbValue() == i
	 *
	 * Every row in the adjacency lists must be sorted in the following order: The first sample value has
	 * the least distance to the source sample value, the last has the largest distance. If two sample values
	 * in one row have the same distance to the source sample value, the order does not matter.
	 *
	 * May be overridden in derived class to provide a faster version.
	 **/
	virtual std::vector<SampleValueAdjacencyList*> calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const ;

	/**
	 * get recommended list of matching algorithms
	 * \param m an empty matching - will be used in construction of MatchingAlgorithm objects
	 *
	 * The MatchingAlgorithm objects returned by this function should be deleted by the caller
	 * if they are no longer needed.
	 **/
	virtual std::vector<MatchingAlgorithm*> getMatchingAlgorithms (Graph* g, Matching* m) const ;

	/**
	 * get the name of this cvrstgfile
	 **/
	const std::string& getName (void) const
		{ return getBinIO()->getName() ; } ;

	bool is_std (void) const
		{ return getBinIO()->is_std() ; } ;

	/**
	 * get the capacity of this cvrstgfile
	 * \return the capacity in bytes
	 **/
	unsigned long getCapacity (void) const ;
	
	/**
	 * get the capacity as a human-readable string
	 **/
	std::string getHRCapacity (void) const ;

	/**
	 * get the number of samples per vertex (this is a file-format specific constant)
	 **/
	unsigned short getSamplesPerVertex (void) const
		{ return SamplesPerVertex ; } ;
	/**
	 * get the neighbourhood radius (this is a file-format specific constant)
	 **/
	UWORD32 getRadius (void) const
		{ return Radius ; } ;
	/**
	 * values that are embedded in samples will be in 0...Modulus-1 (this is a file-format specific constant)
	 **/
	EmbValue getEmbValueModulus (void) const
		{ return EmbValueModulus ; } ;

	/**
	 * get the value that is embedded in the Sample pos
	 * \param pos the position of the sample
	 * \return the value that is embedded in the sample at the given sample position
	 *
	 * This is equivalent to getSample(pos)->getEmbeddedValue() and is implemented here like this.
	 *
	 * May be overwritten by derived class to provide a faster version.
	 **/
	virtual EmbValue getEmbeddedValue (const SamplePos pos) const ;

#ifdef DEBUG
	/**
	 * return a map that contains the absolute sample frequencies indexed by sample key
	 **/
	virtual std::map<SampleKey,unsigned long>* getFrequencies (void) ;
	/**
	 * print the frequency table freqs
	 *
	 * This method is meant to be overriden in *File to provide a file-format specific way
	 * of printing a frequency table.
	 **/
	virtual void printFrequencies (const std::map<SampleKey,unsigned long>& freqs) ;
#endif

	protected:
	void setSamplesPerVertex (unsigned short spv)
		{ SamplesPerVertex = spv ; } ;
	/**
	 * set Radius to r unless Args.Radius is set (set Radius to Args.Radius.getValue() then)
	 **/
	void setRadius (UWORD32 r) ;
	void setEmbValueModulus (EmbValue m)
		{ EmbValueModulus = m ; } ;
	
	void setBinIO (BinaryIO* io)
		{ BinIO = io ; } ;

	BinaryIO* getBinIO (void) const
		{ return BinIO ; } ;

	private:
	enum FILEFORMAT { UNKNOWN, BMP, WAV, AU, JPEG } ;

	/**
	 * guesses the file format by looking at the first few bytes
	 **/
	static FILEFORMAT guessff (BinaryIO *io) ;

	BinaryIO* BinIO ;

	unsigned short SamplesPerVertex ;
	UWORD32 Radius ;
	EmbValue EmbValueModulus ;
} ;

#endif /* ndef SH_CVRSTGFILE_H */

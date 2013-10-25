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

#ifndef SH_AUFILE_H
#define SH_AUFILE_H

#include <vector>

#include "AudioData.h"
#include "CvrStgFile.h"

class BinaryIO ;

/**
 * \class AuFile
 * \brief a CvrStgFile in Sun .au format
 **/
class AuFile : public CvrStgFile {
	public:
	AuFile (BinaryIO *io) ;
	~AuFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	std::list<CvrStgFile::Property> getProperties (void) const ;
	std::vector<MatchingAlgorithm*> getMatchingAlgorithms (Graph* g, Matching* m) const ;

	unsigned long getNumSamples (void) const
		{ return Data->getNumSamples() ; } ;

	void replaceSample (const SamplePos pos, const SampleValue* s)
		{ return Data->replaceSample(pos, s) ; } ;

	SampleValue* getSampleValue (SamplePos pos) const
		{ return Data->getSampleValue(pos) ; } ;

	private:
	enum ENCODING { MULAW8 = 1, PCM8 = 2, PCM16 = 3 } ;
	class AuHeader {
		public:
		char		id[4] ;
		UWORD32		offset ;
		UWORD32		size ;
		ENCODING	encoding ;
		UWORD32		samplerate ;
		UWORD32		channels ;

		static const UWORD32 SizeUnknown = 0xFFFFFFFF ;
		static const unsigned short HeaderSize = 24 ;

		unsigned short getBytesPerSample (void) const ;
	} ;

	static const UWORD32 Radius_MuLaw8 = 1 ;
	static const UWORD32 Radius_PCM8 = 1 ;
	static const UWORD32 Radius_PCM16 = 20 ;
	static const unsigned short SamplesPerVertex = 2 ;
	static const EmbValue EmbValueModulus = 2 ;

	AuHeader Header ;
	std::vector<BYTE> Infofield ;
	AudioData* Data ;
} ;

#endif /* ndef SH_AUFILE_H */

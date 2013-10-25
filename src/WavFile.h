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

#ifndef SH_WAVFILE_H
#define SH_WAVFILE_H

#include <vector>

#include "CvrStgFile.h"

class BinaryIO ;
class WavChunkHeader ;
class WavChunkUnused ;
class WavFormatChunk ;

class WavFile : public CvrStgFile {
	public:
	WavFile (void) ;
	WavFile (BinaryIO *io) ;
	~WavFile (void) ;

	void read (BinaryIO *io) ;
	void write (void) ;

	std::list<CvrStgFile::Property> getProperties (void) const ;

	unsigned long getNumSamples (void) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;
	SampleValue* getSampleValue (SamplePos pos) const ;

	std::vector<SampleValueAdjacencyList*> calcSVAdjacencyLists (const std::vector<SampleValue*>& svs) const ;
	std::vector<MatchingAlgorithm*> getMatchingAlgorithms (Graph* g, Matching* m) const ;

	unsigned short getBitsPerSample (void) const ;

	private:
	static const signed short	FormatPCM = 1 ;

	static const unsigned short SamplesPerVertex = 2 ;
	static const UWORD32 Radius_small = 1 ;
	static const UWORD32 Radius_large = 20 ;
	static const EmbValue EmbValueModulus = 2 ;

	WavChunkHeader *riffchhdr ;
	char id_wave[4] ;

	WavFormatChunk *FormatChunk ;

	WavChunkHeader *datachhdr ;
	/// this std::vector contains the wav data if BitsPerSample <= 8
	std::vector<unsigned char> data_small ;
	/// this std::vector contains the wav data if BitsPerSample >8
	std::vector<int> data_large ;	// it is assumed that an int can hold 32 bits
	
	std::vector<WavChunkUnused*> UnusedBeforeData ;
	std::vector<BYTE> UnusedAfterData ;

	void readheaders (void) ;
	void readdata (void) ;
	void writeheaders (void) ;
	void writedata (void) ;
	void calcpos (SamplePos n, unsigned long *bytepos, unsigned short *firstbitpos) const ;
	/**
	 * get the position of the first bit (of the first byte) containing the actual sample data
	 * \return the bit position (where 0 is the lsb and 7 the msb)
	 **/
	unsigned short getFirstBitPosinSample (void) ;
	unsigned short getBytesPerSample (void) ;
} ;

#endif // ndef SH_WAVFILE_H

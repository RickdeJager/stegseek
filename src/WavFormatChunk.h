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

#ifndef SH_WAVFORMATCHUNK_H
#define SH_WAVFORMATCHUNK_H

#include "common.h"
#include "WavChunk.h"

class BinaryIO ;

class WavFormatChunk : public WavChunk {
	public:
	WavFormatChunk (void) :
		WavChunk() {} ;
	WavFormatChunk (WavChunkHeader *chh) :
		WavChunk(chh) {} ;
	WavFormatChunk (WavChunkHeader *chh, BinaryIO *io) :
		WavChunk(chh) { read(io) ; } ;

	void read (BinaryIO *io) ;
	void write (BinaryIO *io) ;

	UWORD16 getFormatTag (void) const
		{ return FormatTag ; } ;

	UWORD16 getBitsPerSample (void) const
		{ return BitsPerSample ; } ;

	static const UWORD16 FormatPCM = 0x0001 ;

	private:
	UWORD16 FormatTag ;
	UWORD16 Channels ;
	UWORD32 SamplesPerSec ;
	UWORD32 AvgBytesPerSec ;
	UWORD16 BlockAlign ;
	UWORD16 BitsPerSample ;
	UWORD16 AdditionalSize ;
} ;

#endif // ndef SH_WAVCHUNKFORMAT_H

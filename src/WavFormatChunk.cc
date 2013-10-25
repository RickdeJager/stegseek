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

#include "error.h"
#include "BinaryIO.h"
#include "WavChunkHeader.h"
#include "WavFormatChunk.h"

void WavFormatChunk::read (BinaryIO *io)
{
	FormatTag = io->read16_le() ;
	if (FormatTag != FormatPCM) {
		if (io->is_std()) {
			throw NotImplementedError (_("the wav file from standard input has a format that is not supported (FormatTag: 0x%X)."), FormatTag) ;
		}
		else {
			throw NotImplementedError (_("the wav file \"%s\" has a format that is not supported (FormatTag: 0x%X)."), io->getName().c_str(), FormatTag) ;
		}
	}
	Channels = io->read16_le() ;
	SamplesPerSec = io->read32_le() ;
	AvgBytesPerSec = io->read32_le() ;
	BlockAlign = io->read16_le() ;
	BitsPerSample = io->read16_le() ;
	if (ChunkHeader->getChunkLength() == 16) {
		AdditionalSize = 0 ;
	}
	else {
		AdditionalSize = io->read16_le() ;
		if (AdditionalSize != 0) {
			if (io->is_std()) {
				throw SteghideError (_("the wav file from standard input does not have pcm format (header too long).")) ;
			}
			else {
				throw SteghideError (_("the wav file \"%s\" does not have pcm format (header too long)."), io->getName().c_str()) ;
			}
		}
		if (ChunkHeader->getChunkLength() != 18) {
			if (io->is_std()) {
				throw SteghideError (_("the wav file from standard input is corrupted.")) ;
			}
			else {
				throw SteghideError (_("the wav file \"%s\" is corrupted."), io->getName().c_str()) ;
			}
		}
	}
}

void WavFormatChunk::write (BinaryIO *io)
{
	ChunkHeader->write (io) ;
	io->write16_le (FormatTag) ;
	io->write16_le (Channels) ;
	io->write32_le (SamplesPerSec) ;
	io->write32_le (AvgBytesPerSec) ;
	io->write16_le (BlockAlign) ;
	io->write16_le (BitsPerSample) ;
	if (ChunkHeader->getChunkLength() == 18) {
		io->write16_le (AdditionalSize) ;
	}
}

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

#include "BinaryIO.h"
#include "WavChunkHeader.h"

WavChunkHeader::WavChunkHeader (char *id, UWORD32 len)
{
	for (unsigned int i = 0 ; i < 4 ; i++) {
		ChunkId[i] = id[i] ;
	}
	ChunkLength = len ;
}

WavChunkHeader::WavChunkHeader (BinaryIO *io)
{
	read (io) ;
}

void WavChunkHeader::read (BinaryIO *io)
{
	for (unsigned int i = 0 ; i < 4 ; i++) {
		ChunkId[i] = io->read8() ;
	}
	ChunkLength = io->read32_le() ;
}

void WavChunkHeader::write (BinaryIO *io) const
{
	for (unsigned int i = 0 ; i < 4 ; i++) {
		io->write8 (ChunkId[i]) ;
	}
	io->write32_le (ChunkLength) ;
}

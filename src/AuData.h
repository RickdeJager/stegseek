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

#ifndef SH_AUDATA_H
#define SH_AUDATA_H

#include "BinaryIO.h"
#include "AudioData.h"

// AuMuLawAudioData
typedef AudioDataImpl<AuMuLaw,BYTE> AuMuLawAudioData ;
template<> inline BYTE AuMuLawAudioData::readValue (BinaryIO* io) const { return (io->read8()) ; }
template<> inline void AuMuLawAudioData::writeValue (BinaryIO* io, BYTE v) const { io->write8(v) ; }

// AuPCM8AudioData
typedef AudioDataImpl<AuPCM8,SBYTE> AuPCM8AudioData ;
template<> inline SBYTE AuPCM8AudioData::readValue (BinaryIO* io) const { return ((SBYTE) io->read8()) ; }
template<> inline void AuPCM8AudioData::writeValue (BinaryIO* io, SBYTE v) const { io->write8((BYTE) v) ; }

// AuPCM16AudioData
typedef AudioDataImpl<AuPCM16,SWORD16> AuPCM16AudioData ;
template<> inline SWORD16 AuPCM16AudioData::readValue (BinaryIO* io) const { return ((SWORD16) io->read16_be()) ; }
template<> inline void AuPCM16AudioData::writeValue (BinaryIO* io, SWORD16 v) const { io->write16_be((UWORD16) v) ; }

// AuPCM32AudioData
typedef AudioDataImpl<AuPCM32,SWORD32> AuPCM32AudioData ;
template<> inline SWORD32 AuPCM32AudioData::readValue (BinaryIO* io) const { return ((SWORD32) io->read32_be()) ; }
template<> inline void AuPCM32AudioData::writeValue (BinaryIO* io, SWORD32 v) const { io->write32_be((UWORD32) v) ; }

#endif // ndef SH_AUDATA_H

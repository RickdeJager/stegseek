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

#include "AuSampleValues.h"

// AuMuLawSampleValue
template<> const BYTE AuMuLawSampleValue::MinValue = 0 ;
template<> const BYTE AuMuLawSampleValue::MaxValue = BYTE_MAX ;

// AuPCM8SampleValue
template<> const SBYTE AuPCM8SampleValue::MinValue = SBYTE_MIN ;
template<> const SBYTE AuPCM8SampleValue::MaxValue = SBYTE_MAX ;

// AuPCM16SampleValue
template<> const SWORD16 AuPCM16SampleValue::MinValue = SWORD16_MIN ;
template<> const SWORD16 AuPCM16SampleValue::MaxValue = SWORD16_MAX ;

// AuPCM32SampleValue
template<> const SWORD32 AuPCM32SampleValue::MinValue = SWORD32_MIN ;
template<> const SWORD32 AuPCM32SampleValue::MaxValue = SWORD32_MAX ;

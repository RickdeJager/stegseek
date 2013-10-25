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

#ifndef SH_COMMON_H
#define SH_COMMON_H

//
// this file contains some definitions that are/can be used throughout the whole program
//

//
// include config.h
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//
// generic type definitions
#ifdef HAVE_CONFIG_H
typedef TYPE_UWORD32	UWORD32 ;
typedef TYPE_UWORD16	UWORD16 ;
typedef TYPE_BYTE		BYTE ;
typedef TYPE_SWORD32	SWORD32 ;
typedef TYPE_SWORD16	SWORD16 ;
typedef TYPE_SBYTE		SBYTE ;
#else
// use types that are at least the correct size
typedef unsigned long	UWORD32 ;
typedef unsigned short	UWORD16 ;
typedef unsigned char	BYTE ;
typedef long			SWORD32 ;
typedef short			SWORD16 ;
typedef char			SBYTE ;
#endif

#define UWORD32_MAX		0xFFFFFFFFUL
#define UWORD16_MAX		0xFFFF
#define BYTE_MAX		0xFF
#define SWORD32_MIN		-2147483647 - 1 // to avoid a gcc warning (caused by an overflow)
#define SWORD32_MAX		2147483647
#define SWORD16_MIN		-32768
#define SWORD16_MAX		32767
#define SBYTE_MIN		-128
#define SBYTE_MAX		127

//
// specialised type definitions
typedef bool BIT ;
typedef BYTE	EmbValue ;
typedef UWORD32 SamplePos ;
typedef UWORD32 VertexLabel ;
typedef UWORD32 SampleValueLabel ;
typedef UWORD32 SampleKey ;

#define SAMPLEKEY_MAX UWORD32_MAX
#define VERTEXLABEL_MAX UWORD32_MAX

//
// gettext support
#include "gettext.h" 
#define _(S) gettext (S)

//
// global variables
#include "Globals.h"
extern Globals Globs ;

//
// every class should (be able to) do assertions
// (myassert(expr) is more verbose than the standard C assert)
#include "AssertionFailed.h"
#define myassert(expr)		if (!(expr)) throw AssertionFailed (__FILE__, __LINE__)

//
// every class can have debugging output
#include <iostream> // for verbose testing output
#if DEBUG
#define RUNDEBUGLEVEL(LEVEL) ((LEVEL) <= Args.DebugLevel.getValue())
extern void printDebug (unsigned short level, const char *msgfmt, ...) ;
#endif

//
// every class has access to the command line arguments
#include "Arguments.h"
extern Arguments Args ;

//
// every class has access to random data
#include "RandomSource.h"
extern RandomSource RndSrc ;

#endif // ndef SH_COMMON_H

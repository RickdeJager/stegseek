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

#ifndef SH_RGBTRIPLE_H
#define SH_RGBTRIPLE_H

#include "common.h"

class RGBTriple {
	public:
	RGBTriple (void)
		: Red(0), Green(0), Blue(0) {} ;
	RGBTriple (BYTE r, BYTE g, BYTE b)
		: Red(r), Green(g), Blue(b) {} ;

	/**
	 * get the squared distance in the RGB cube between this triple and the triple t
	 * \param t another RGB triple
	 * \return the square of the euclidean distance between this and t
	 **/
	UWORD32 calcDistance (const RGBTriple& t) const ;

	/**
	 * returns true iff this triple and t are equal (i.e. have the same rgb values)
	 **/
	bool operator== (const RGBTriple& t) const ;

	/**
	 * return true iff this triple and t are not equal (i.e. have different rgb values)
	 **/
	bool operator!= (const RGBTriple& t) const ;

	BYTE Red ;
	BYTE Green ;
	BYTE Blue ;
} ;

#endif // ndef SH_RGBTRIPLE_H

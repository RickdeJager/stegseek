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

#ifndef SH_COLORPALETTE_H
#define SH_COLORPALETTE_H

#include <vector>

#include "RGBTriple.h"
#include "common.h"

/**
 * \class ColorPalette
 * \brief a color palette
 *
 * This class is essentially a vector<RGBTriple> with
 * some wrappers for exisiting methods.
 **/
class ColorPalette : public std::vector<RGBTriple> {
	public:
	/**
	 * get the size, i.e. the number of entries of this color palette
	 **/
	unsigned int getSize (void) const
		{ return size() ; } ;

	/**
	 * add (a copy of) rgb to the end of this color palette
	 **/
	void addEntry (RGBTriple rgb)
		{ push_back (rgb) ; } ;

	/**
	 * add the color r/g/b to the end of this color palette
	 **/
	void addEntry (BYTE r, BYTE g, BYTE b)
		{ push_back (RGBTriple (r, g, b)) ; } ;
} ;

#endif // ndef SH_COLORPALETTE_H

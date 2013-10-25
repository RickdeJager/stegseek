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

#ifndef SH_ASSERTIONFAILED_H
#define SH_ASSERTIONFAILED_H

#include "common.h"
#include "SteghideError.h"

class AssertionFailed : public SteghideError {
	public:
	AssertionFailed (const char* fn, unsigned int l)
		: SteghideError(_("assertion failed in %s at line number %d."), stripDir(fn), l) {} ;

	void printMessage (void) const ;

	private:
	char* stripDir (const char* fn) ;
} ;

#endif // ndef SH_ASSERTION_FAILED

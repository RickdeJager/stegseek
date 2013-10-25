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

#include "BmpSampleValue.h"
#include "common.h"

UWORD32 BmpSampleValue::calcDistance (const SampleValue *s) const
{
	const BmpSampleValue *sample = (const BmpSampleValue*) s ;
	/* If s is not a BmpSampleValue then we get into real trouble here.
	But calcDistance is called very often, a dynamic_cast costs a lot of time and
	it does not make sense to pass anything but a BmpSampleValue as s anyway. */

	int dr = (int) getRed() - (int) sample->getRed() ;
	int dg = (int) getGreen() - (int) sample->getGreen() ;
	int db = (int) getBlue() - (int) sample->getBlue() ;
	return (UWORD32) (dr*dr + dg*dg + db*db) ;
}

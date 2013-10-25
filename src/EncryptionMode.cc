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

#include "common.h"
#include "EncryptionMode.h"

EncryptionMode::EncryptionMode ()
{
	setValue (ECB) ;
}

EncryptionMode::EncryptionMode (EncryptionMode::IRep irep)
{
	setValue (irep) ;
}

EncryptionMode::EncryptionMode (std::string srep)
{
	setValue (translate (srep)) ;
}

void EncryptionMode::setValue (EncryptionMode::IRep irep)
{
	Value = irep ;
}

std::string EncryptionMode::getStringRep () const
{
	return translate(Value) ;
}

EncryptionMode::IRep EncryptionMode::getIntegerRep () const
{
	return Value ;
}

bool EncryptionMode::isValidStringRep (std::string srep)
{
	bool retval = false ;
	for (unsigned int i = 0 ; i < NumValues ; i++) {
		if (Translations[i].srep == srep) {
			retval = true ;
		}
	}
	return retval ;
}

bool EncryptionMode::isValidIntegerRep (unsigned int irep)
{
	return (irep < NumValues) ;
}

std::string EncryptionMode::translate (EncryptionMode::IRep irep)
{
	std::string retval ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumValues ; i++) {
		if (Translations[i].irep == irep) {
			retval = std::string (Translations[i].srep) ;
			found = true ;
		}
	}
	myassert (found) ;
	return retval ;
}

EncryptionMode::IRep EncryptionMode::translate (std::string srep)
{
	IRep retval = ECB ;
	bool found = false ;
	for (unsigned int i = 0 ; i < NumValues ; i++) {
		if (Translations[i].srep == srep) {
			retval = Translations[i].irep ;
			found = true ;
		}
	}
	myassert (found) ;
	return retval ;
}

const EncryptionMode::Translation EncryptionMode::Translations[] = {
	{ ECB, "ecb" },
	{ CBC, "cbc" },
	{ OFB, "ofb" },
	{ CFB, "cfb" },
	{ NOFB, "nofb" },
	{ NCFB, "ncfb" },
	{ CTR, "ctr" },
	{ STREAM, "stream" }
} ;

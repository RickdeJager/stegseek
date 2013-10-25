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
#include "EncryptionAlgorithm.h"

EncryptionAlgorithm::EncryptionAlgorithm ()
{
	setValue (NONE) ;
}

EncryptionAlgorithm::EncryptionAlgorithm (EncryptionAlgorithm::IRep irep)
{
	setValue (irep) ;
}

EncryptionAlgorithm::EncryptionAlgorithm (std::string srep)
{
	setValue (translate (srep)) ;
}

void EncryptionAlgorithm::setValue (EncryptionAlgorithm::IRep irep)
{
	Value = irep ;
}

std::string EncryptionAlgorithm::getStringRep () const
{
	return translate(Value) ;
}

EncryptionAlgorithm::IRep EncryptionAlgorithm::getIntegerRep () const
{
	return Value ;
}

bool EncryptionAlgorithm::isValidStringRep (std::string srep)
{
	bool retval = false ;
	for (unsigned int i = 0 ; i < NumValues ; i++) {
		if (Translations[i].srep == srep) {
			retval = true ;
		}
	}
	return retval ;
}

bool EncryptionAlgorithm::isValidIntegerRep (unsigned int irep)
{
	return (irep < NumValues) ;
}

std::string EncryptionAlgorithm::translate (EncryptionAlgorithm::IRep irep)
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

EncryptionAlgorithm::IRep EncryptionAlgorithm::translate (std::string srep)
{
	IRep retval = NONE ;
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

const EncryptionAlgorithm::Translation EncryptionAlgorithm::Translations[] = {
	{ NONE, "none" },
	{ TWOFISH, "twofish" },
	{ RIJNDAEL128, "rijndael-128" },
	{ RIJNDAEL192, "rijndael-192" },
	{ RIJNDAEL256, "rijndael-256" },
	{ SAFERPLUS, "saferplus" },
	{ RC2, "rc2" },
	{ XTEA, "xtea" },
	{ SERPENT, "serpent" },
	{ SAFERSK64, "safer-sk64" },
	{ SAFERSK128, "safer-sk128" },
	{ CAST256, "cast-256" },
	{ LOKI97, "loki97" },
	{ GOST, "gost" },
	{ THREEWAY, "threeway" },
	{ CAST128, "cast-128" },
	{ BLOWFISH, "blowfish" },
	{ DES, "des" },
	{ TRIPLEDES, "tripledes" },
	{ ENIGMA, "enigma" },
	{ ARCFOUR, "arcfour" },
	{ PANAMA, "panama" },
	{ WAKE, "wake" }
} ;

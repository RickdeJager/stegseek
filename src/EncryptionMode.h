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

#ifndef SH_ENCMODE_H
#define SH_ENCMODE_H

#include <string>

class EncryptionMode {
	public:
	/// number of bits needed to code the mode
	static const unsigned int IRep_size = 3 ;

	/// integer representation of encryption mode
	enum IRep {
		ECB = 0,
		CBC = 1,
		OFB = 2,
		CFB = 3,
		NOFB = 4,
		NCFB = 5,
		CTR = 6,
		STREAM = 7
	} ;

	/**
	 * construct a new EncryptionMode object setting Value to ECB
	 **/
	EncryptionMode (void) ;
	EncryptionMode (IRep irep) ;
	/**
	 * construct a new EncryptionMode object from a std::string representation
	 * \param srep a valid(!) std::string representation
	 **/
	EncryptionMode (std::string srep) ;

	void setValue (IRep irep) ;

	std::string getStringRep (void) const ;
	IRep getIntegerRep (void) const ;

	bool operator== (const EncryptionMode& mode) const
		{ return (Value == mode.Value) ; } ;

	static bool isValidStringRep (std::string srep) ;
	static bool isValidIntegerRep (unsigned int irep) ;

	static std::string translate (IRep irep) ;
	static IRep translate (std::string srep) ;

	private:
	static const unsigned int NumValues = 8 ;
	IRep Value ;

	typedef struct struct_Translation {
		IRep	irep ;
		char*	srep ;
	} Translation ;
	static const Translation Translations[] ;
} ;

#endif // ndef SH_ENCMODE_H

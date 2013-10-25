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

#ifndef SH_ENCALGO_H
#define SH_ENCALGO_H

#include <string>

class EncryptionAlgorithm {
	public:
	/// number of bits needed to code the algorithm	
	static const unsigned int IRep_size = 5 ;

	/// integer representation of encryption algorithm
	enum IRep {
		NONE = 0,
		TWOFISH = 1,
		RIJNDAEL128 = 2,
		RIJNDAEL192 = 3,
		RIJNDAEL256 = 4,
		SAFERPLUS = 5,
		RC2 = 6,
		XTEA = 7,
		SERPENT = 8,
		SAFERSK64 = 9,
		SAFERSK128 = 10,
		CAST256 = 11,
		LOKI97 = 12,
		GOST = 13,
		THREEWAY = 14,
		CAST128 = 15,
		BLOWFISH = 16,
		DES = 17,
		TRIPLEDES = 18,
		ENIGMA = 19,
		ARCFOUR = 20,
		PANAMA = 21,
		WAKE = 22
	} ;

	EncryptionAlgorithm (void) ;
	EncryptionAlgorithm (IRep irep) ;
	/**
	 * construct a new EncryptionAlgorithm object from a std::string representation
	 * \param srep a valid(!) std::string representation
	 **/
	EncryptionAlgorithm (std::string srep) ;

	void setValue (IRep irep) ;

	std::string getStringRep (void) const ;
	IRep getIntegerRep (void) const ;

	bool operator== (const EncryptionAlgorithm& algo) const
		{ return (Value == algo.Value) ; } ;

	/**
	 * check if srep is a valid std::string representation (w.r.t the Translations array)
	 * \param srep a std::string that maybe represents an encryption algorithm fron the Translations table
	 * \return true iff the Translations table contains srep
	 **/
	static bool isValidStringRep (std::string srep) ;

	static bool isValidIntegerRep (unsigned int irep) ;

	/**
	 * translate an integer representation into the corresponding std::string representation
	 **/
	static std::string translate (IRep irep) ;

	/**
	 * translate a valid std::string representation into the corresponding integer representation
	 **/
	static IRep translate (std::string srep) ;

	private:
	static const unsigned int NumValues = 23 ;
	IRep Value ;

	typedef struct struct_Translation {
		IRep	irep ;
		char*	srep ;
	} Translation ;
	static const Translation Translations[] ;
} ;

#endif // ndef SH_ENCALGO_H

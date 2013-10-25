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

#ifndef SH_BITSTRING_H
#define SH_BITSTRING_H

#include <vector>
#include <string>

#include "AUtils.h"
#include "common.h"

/**
 * \class BitString
 * \brief a string of bits
 *
 * This class provides a way to conveniently store and
 * manipulate a string of bits. Various objects can be
 * appended to a BitString.
 * 
 * For data storage a vector of BYTEs is used with little
 * endian bit encoding, i.e. the first bit is the least
 * significant bit of the first byte and so on.
 **/
class BitString {
	public:
	/**
	 * construct an empty BitString
	 **/
	BitString (EmbValue arity = 2) ;
	
	/**
	 * copy constructor
	 **/
	BitString (const BitString& bs) ;
	
	/**
	 * construct a BitString containing l bits with value zero
	 **/
	BitString (const unsigned long l) ;
	
	/**
	 * construct a BitString containing the data in d
	 **/
	BitString (const std::vector<BYTE>& d) ;
	
	/**
	 * construct a BitString containing the characters in d as 8 bit unsigned chars
	 **/
	BitString (const std::string& d) ;

	void setArity (EmbValue arity) ;

	EmbValue getArity (void) const
		{ return Arity ; } ;

	/**
	 * get the number of bits in this BitString
	 **/
	UWORD32 getLength (void) const
		{ return Length ; } ;

	/**
	 * get the number of EmbValues in this BitString (using this BitString's arity)
	 **/
	UWORD32 getNAryLength (void) const
		{ return AUtils::div_roundup<UWORD32> (Length, ArityNBits) ; } ;

	/**
	 * delete the contents of this Bitstring
	 **/
	BitString& clear (void) ;

	/**
	 * append the bit v to this BitString
	 **/
	BitString& append (const BIT v) ;

	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const BYTE v, const unsigned short n = 8) ;
	
	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const UWORD16 v, const unsigned short n = 16) ;
	
	/**
	 * append n lower order bits of v to this BitString
	 * \param v the value to be appended
	 * \param n the number of bits to be appended
	 **/
	BitString& append (const UWORD32 v, const unsigned short n = 32) ;
	
	/**
	 * append the string v to this BitString
	 **/
	BitString& append (const std::string& v) ;

	/**
	 * append the vector v byte-wise to this BitString
	 **/
	BitString& append (const std::vector<BYTE>& v) ;

	/**
	 * append the BitString v to this BitString
	 * \param v the BitString to be appended
	 **/
	BitString& append (const BitString& v) ;

	/**
	 * set the p-th bit to v
	 * \param i the index (must be < getLength())
	 * \param v the value
	 **/
	BitString& setBit (unsigned long i, BIT v) ;

	/**
	 * get a BitString that is a part of this BitString
	 * \param s the index of the first bit to be copied from this BitString
	 * \param l the total number of bits to be used for the return value
	 * \return the BitString containing of the bits [s...s+(l-1)] of this BitString
	 **/
	BitString getBits (const unsigned long s, const unsigned long l) const ;

	/**
	 * cut some bits out of this BitString
	 * \param s the index of the first bit to be removed from this BitString
	 * \param l the total number of bits to be removed
	 * \return the BitString containing of the bits [s...s+(l-1)] of this BitString
	 *
	 * After calling cutBits, this BitString consists of the bits 0,...,s-1,s+l,... .
	 **/
	BitString cutBits (const unsigned long s, const unsigned long l) ;

	/**
	 * return a value composed from bits in this BitString
	 * \param s the index of the first bit to be used for the return value
	 * \param l the total number of bits to be used for the return value (must be <= 32)
	 * \return the value (*this)[s],...,(*this)[s+l-1]
	 **/
	UWORD32 getValue (const unsigned long s, const unsigned short l) const ;

	/**
	 * get the contents of this BitString as vector of bytes
	 * \return the contents of this BitString as vector of bytes
	 *
	 * getLength() % 8 must be 0 to call this function
	 **/
	const std::vector<BYTE>& getBytes (void) const ;

	/**
	 * truncate this BitString
	 * \param s start
	 * \param e end
	 * \return this BitString modified to contain only (*this)[s]...(*this)[e - 1]
	 **/
	BitString& truncate (const unsigned long s, const unsigned long e) ;

	/**
	 * pad this BitString with the value in v
	 * \param mult this BitString is padded until size is a multiple of mult (given in bits)
	 * \param v the value this BitString should be padded with
	 **/
	BitString& pad (const unsigned long mult, const BIT v) ;

	/**
	 * pad this BitString with random data
	 * \param mult this BitString is padded until size is a multiple of mult (given in bits)
	 **/
	BitString& padRandom (const unsigned long mult) ;

	/**
	 * get an n-ary digit from this BitString
	 * \param p the position (in the n-ary representation of this BitString)
	 * \return the p-th n-ary digit
	 **/
	BYTE getNAry (unsigned long p) const ;

	/**
	 * append an n-ary digit to this BitString
	 * \param v the n-ary value to be appended
	 **/
	void appendNAry (BYTE v) ;

#ifdef USE_ZLIB
	/**
	 * compress this BitString using zlib's compress2 function
	 * \param level the compression level (ranging from 1 (best speed) to 9 (best compression)
	 *
	 * As zlib operates on byte vectors and not on bit vectors, this BitString is padded
	 * with zeros until the next byte boundary before starting the compression.
	 **/
	BitString& compress (int level) ;

	/**
	 * uncompress this BitString using zlib's uncompress function
	 * \param idestlen the length of the uncompressed data (in bits)
	 * \return this BitString modified to contain exactly idestlen uncompressed bits
	 *
	 * idestlen must be the size of the uncompressed data (in bits) to make
	 * it possible to allocate a destination buffer and to give information
	 * about the length of the uncompressed data without padding.
	 *
	 * A precondition for calling this function is that this BitString actually
	 * contains data that has been compressed by the BitString::compress function,
	 * and especially that the Length % 8 is 0.
	 **/
	BitString& uncompress (unsigned long idestlen) ;
#endif // def USE_ZLIB

	/**
	 * get the value of the i-th bit
	 **/
	BIT operator[] (const unsigned long i) const ;

	/**
	 * xor v with this BitString, saving the result in this Bitstring.
	 * The result has the same length as this BitString.
	 **/
	BitString& operator^= (const BitString &v) ;

	/**
	 * compare this BitString with the BitString v
	 * \return true iff the lengths are equal and for every valid index the value is equal
	 **/
	bool operator== (const BitString& v) const ;

	/**
	 * compare this BitString with the BitString v
	 * \return true iff the lengths are not equal or there exists an index with different values
	 **/
	bool operator!= (const BitString& v) const ;

	void print (unsigned short spc = 0) const ;
#ifdef DEBUG
	void printDebug (unsigned short level, unsigned short spc = 0) const ;
#endif

	private:
	/// the number of bits in Data
	UWORD32 Length ;
	/// the arity that will be used for getLength/getNAry/appendNAry
	EmbValue Arity ;
	/// the number of Bits per n-ary digit (where n is Arity)
	unsigned short ArityNBits ;
	/// the actual data
	std::vector<BYTE> Data ;

	void _append (BIT v) ;

	/**
	 * clear unused part of last byte (_append depends on this)
	 **/
	void clearUnused (void) ;
} ;

#endif // ndef SH_BITSTRING_H

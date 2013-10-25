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

#include "AUtils.h"
#include "BitString.h"
#include "common.h"

#ifdef USE_ZLIB
namespace zlib {
#include <zlib.h>
}
#endif // def USE_ZLIB

#define BITPOS(n) (n % 8)
#define BYTEPOS(n) (n / 8)

BitString::BitString (EmbValue arity)
	: Length(0)
{
	setArity(arity) ;
}

BitString::BitString (const BitString& bs)
{
	Length = bs.Length ;
	Arity = bs.Arity ;
	ArityNBits = bs.ArityNBits ;
	Data.resize(bs.Data.size()) ;
	for (unsigned long i = 0 ; i < bs.Data.size() ; i++) {
		Data[i] = bs.Data[i] ;
	}
}

BitString::BitString (unsigned long l)
{
	unsigned long nbytes = 0 ;
	if (l % 8 == 0) {
		nbytes = l / 8 ;
	}
	else {
		nbytes = (l / 8) + 1 ;
	}

	Data = std::vector<BYTE> (nbytes, 0) ; // is initialized to zeros in std::vector's constructor
	Length = l ;
	setArity(2) ;
}

BitString::BitString (const std::vector<BYTE> &d)
	: Length(0)
{
	setArity(2) ;
	append (d) ;
}

BitString::BitString (const std::string &d)
	: Length(0)
{
	setArity(2) ;
	append (d) ;
}

void BitString::setArity (EmbValue arity)
{
	BYTE tmp = Arity = arity ;
	ArityNBits = 0 ;
	while (tmp > 1) {
		myassert (tmp % 2 == 0) ; // only implemented for arity = 2^i
		tmp /= 2 ;
		ArityNBits++ ;
	}
}

BitString& BitString::clear()
{
	Data.clear() ;
	Length = 0 ;
	return *this ;
}

void BitString::_append (BIT v)
{
	if (Length % 8 == 0) {
		Data.push_back (0) ;
	}
	Data[BYTEPOS(Length)] |= (v << BITPOS(Length)) ;
	Length++ ;
}

BitString& BitString::append (BIT v)
{
	_append (v) ;
	return *this ;
}

BitString& BitString::append (const BYTE v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const UWORD16 v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const UWORD32 v, const unsigned short n)
{
	for (unsigned short i = 0 ; i < n ; i++) {
		_append ((BIT) ((v & (1 << i)) >> i)) ;
	}
	return *this ;
}

BitString& BitString::append (const BitString& v)
{
	for (unsigned long i = 0 ; i < v.getLength() ; i++) {
		_append (v[i]) ;
	}
	return *this ;
}

BitString& BitString::append (const std::vector<BYTE>& v)
{
	for (std::vector<BYTE>::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append (*i) ;
	}
	return *this ;
}

BitString& BitString::append (const std::string& v)
{
	for (std::string::const_iterator i = v.begin() ; i != v.end() ; i++) {
		append ((BYTE) *i) ;
	}
	return *this ;
}

BIT BitString::operator[] (unsigned long i) const
{
	myassert (i < Length) ;
	return ((Data[BYTEPOS(i)] >> BITPOS(i)) & 1) ;
}

BitString& BitString::setBit (unsigned long i, BIT v)
{
	myassert (i < Length) ;
	BYTE mask = ~(1 << BITPOS(i)) ;
	Data[BYTEPOS(i)] &= mask ; 
	Data[BYTEPOS(i)] |= (v << BITPOS(i)) ;
	return (*this) ;
}

BitString BitString::getBits (unsigned long s, unsigned long l) const
{
	BitString retval ;
	for (unsigned long i = s ; i < s + l ; i++) {
		retval.append ((*this)[i]) ;
	}
	return retval ;
}

BitString BitString::cutBits (unsigned long s, unsigned long l)
{
	myassert (s + l <= Length) ;
	BitString retval ;
	for (unsigned long i = s, j = s + l ; i < s + l || j < Length ; i++, j++) {
		if (i < s + l) {
			retval.append ((*this)[i]) ;
		}
		if (j < Length) {
			setBit(i, (*this)[j]) ;
		}
	}
	Length -= l ;
	Data.resize (AUtils::div_roundup<UWORD32> (Length, 8)) ;
	clearUnused() ;

	return retval ;
}

UWORD32 BitString::getValue (unsigned long s, unsigned short l) const
{
	myassert (l <= 32) ;
	UWORD32 retval = 0 ;
	for (unsigned short i = 0 ; i < l ; i++) {
		retval |= (*this)[s + i] << i ;
	}
	return retval ;
}

const std::vector<BYTE>& BitString::getBytes() const
{
	myassert (Length % 8 == 0) ;
	return Data ;
}

BitString& BitString::truncate (const unsigned long s, const unsigned long e)
{
	unsigned long newsize_i = e - s ;
	unsigned long newsize_y = 0 ;
	if (newsize_i % 8 == 0) {
		newsize_y = newsize_i / 8 ;
	}
	else {
		newsize_y = newsize_i / 8 + 1 ;
	}

	for (unsigned long i = 0 ; i < newsize_i ; i++) {
		// replace i-th bit with (s+i)-th bit
		BIT bit = (*this)[s + i] ;
		Data[BYTEPOS(i)] &= (BYTE) ~(1 << BITPOS(i)) ; // delete i-th bit
		Data[BYTEPOS(i)] |= (BYTE) (bit << BITPOS(i)) ;
	}
	Length = newsize_i ;

	clearUnused() ;

	return *this ;
}

BitString& BitString::pad (unsigned long mult, BIT v)
{
	while (Length % mult != 0) {
		_append (v) ;
	}
	return *this ;
}

BitString& BitString::padRandom (unsigned long mult)
{
	while (Length % mult != 0) {
		append (RndSrc.getBool()) ;
	}
	return *this ;
}

BYTE BitString::getNAry (unsigned long p) const
{
	unsigned long pbinary = p * ArityNBits ;
	BYTE retval = 0 ;
	for (unsigned short i = 0 ; (i < ArityNBits) && (pbinary + i < Length /* Length not a multiple of ArityNBits */) ; i++) {
		retval |= (*this)[pbinary + i] << i ;
	}
	return retval ;
}

void BitString::appendNAry (BYTE v)
{
	for (unsigned short i = 0 ; i < ArityNBits ; i++) {
		_append ((v & (1 << i)) >> i) ;
	}
}

#ifdef USE_ZLIB
BitString& BitString::compress (int level)
{
	myassert (level >= 1 && level <= 9) ;
	pad (8, 0) ;

	// prepare source buffer
	zlib::uLong srclen = Data.size() ;
	zlib::Byte *srcbuf = new zlib::Byte[srclen] ;
	for (unsigned long i = 0 ; i < srclen ; i++) {
		srcbuf[i] = Data[i] ;
	}

	// prepare dest buffer (see zlib manual for this magic length)
	zlib::uLong destlen = ((zlib::uLong) ((Data.size() + 12) * 1.001)) + 1 ;
	zlib::Byte* destbuf = new zlib::Byte[destlen] ;

	// do compression
	int ret = zlib::compress2 (destbuf, &destlen, (const zlib::Bytef*) srcbuf, srclen, level) ;
	if (ret != Z_OK) {
		switch (ret) {
			case Z_MEM_ERROR:
			throw SteghideError (_("could not allocate memory.")) ;
			break ;

			default:
			throw SteghideError (_("error %d while calling zlib's compress2."), ret) ;
			break ;
		}
	}

	// write compressed data into Data
	Data.resize (destlen) ;
	for (unsigned long i = 0 ; i < destlen ; i++) {
		Data[i] = destbuf[i] ;
	}
	Length = destlen * 8 ;

	delete[] srcbuf ;
	delete[] destbuf ;
	return *this ;
}

BitString& BitString::uncompress (unsigned long idestlen)
{
	myassert (Length % 8 == 0) ;

	// prepare source buffer
	zlib::uLong srclen = Length / 8 ;
	zlib::Byte* srcbuf = new zlib::Byte[srclen] ;
	for (unsigned long i = 0 ; i < srclen ; i++) {
		srcbuf[i] = Data[i] ;
	}

	// prepare destination buffer
	zlib::uLong ydestlen = 0 ;
	if (idestlen % 8 == 0) {
		ydestlen = idestlen / 8 ;
	}
	else {
		ydestlen = (idestlen / 8) + 1 ;
	}
	zlib::Byte* destbuf = new zlib::Byte[ydestlen] ;

	// do uncompression
	zlib::uLong ydestlen_before = ydestlen ;
	int ret = zlib::uncompress (destbuf, &ydestlen, (const zlib::Bytef*) srcbuf, srclen) ;
	if (ret != Z_OK) {
		switch (ret) {
			case Z_MEM_ERROR:
			throw SteghideError (_("could not allocate memory.")) ;
			break ;

			case Z_DATA_ERROR:
			throw SteghideError (_("can not uncompress data. compressed data is corrupted.")) ;
			break ;

			default:
			throw SteghideError (_("error %d while calling zlib's uncompress."), ret) ;
			break ;
		}
	}
	myassert (ydestlen_before == ydestlen) ; // because idestlen must be _exactly_ size in bits

	// write uncompressed data into Data
	Data.resize (ydestlen) ;
	for (unsigned long i = 0 ; i < ydestlen ; i++) {
		Data[i] = destbuf[i] ;
	}
	Length = idestlen ;

	clearUnused() ;

	delete[] srcbuf ;
	delete[] destbuf ;
	return *this ;
}
#endif // def USE_ZLIB

bool BitString::operator== (const BitString &v) const
{
	bool retval = true ;

	if (v.getLength() == getLength()) {
		unsigned int n = getLength() ;
		for (unsigned int i = 0 ; i < n ; i++) {
			if (v[i] != (*this)[i]) {
				retval = false ;
			}
		}
	}
	else {
		retval = false ;
	}
	
	return retval ;
}

bool BitString::operator!= (const BitString &v) const
{
	bool retval = false ;

	if (v.getLength() != getLength()) {
		retval = true ;
	}
	else {
		for (unsigned int i = 0 ; i < getLength() ; i++) {
			if (v[i] != (*this)[i]) {
				retval = true ;
				break ;
			}
		}
	}

	return retval ;
}

BitString& BitString::operator^= (const BitString &v)
{
	for (unsigned long i = 0 ; i < Length ; i++) {
		unsigned long bytepos = BYTEPOS (i) ;
		unsigned int bitpos = BITPOS (i) ;
		BIT bit = (Data[bytepos] & (1 << bitpos)) >> bitpos ;
		bit ^= v[i] ;
		Data[bytepos] = (Data[bytepos] & ~(1 << bitpos)) | (bit << bitpos) ;
	}
	return *this ;
}

void BitString::clearUnused ()
{
	if (Length % 8 != 0) {
		// clear unused part of last byte (_append depends on this)
		unsigned short nbitsfilled = Length % 8 ;
		BYTE mask = 0x00 ;
		for (unsigned short i = 0 ; i < nbitsfilled ; i++) {
			mask <<= 1 ;
			mask |= 1 ;
		}
		Data[Data.size() - 1] &= mask ;
	}
}

void BitString::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;
	std::cerr << space << "Length: " << getLength() << std::endl ;
	std::cerr << space << "Data.size(): " << Data.size() << std::endl ;
	std::cerr << space << "Data:" << std::endl ;

	std::cerr << space ;
	for (unsigned long i = 0 ; i < getLength() ; i++) {
		if ((i % 64 == 0) && (i > 0)) { // new line
			std::cerr << std::endl << space ;
		}
		if (i % 8 == 0) { // new byte
			std::cerr << " " ;
		}
		std::cerr << ((*this)[i]) ;
	}
	std::cerr << std::endl ;
}

#ifdef DEBUG
void BitString::printDebug (unsigned short level, unsigned short spc) const
{
	if (RUNDEBUGLEVEL(level)) {
		print (spc) ;
	}
}
#endif

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

#ifndef SH_AUTILS_H
#define SH_AUTILS_H

#include <cmath>
#ifndef log2
	// this is in an #ifndef because some cmath implementations #define log2 and some not
#	define log2(x) (log(x) / log(2.0))
#endif

/**
 * \class AUtils
 * \brief provides some generic functions for non-standard arithmetic operations
 **/
class AUtils {
	public:
	/**
	 * return the maximum of a and b (needs >)
	 **/
	template<class T> static T max (T a, T b) ;

	/**
	 * return the minimum of a and b (needs <)
	 **/
	template<class T> static T min (T a, T b) ;

	/**
	 * returns a divided through b rounded up to nearest "integer" (needs =, --, +, /)
	 **/
	template<class T> static T div_roundup (T a, T b) ;

	/**
	 * substraction with the modification to return 0 (T()) for negative difference (needs >, -, T())
	 **/
	template<class T> static T bminus (T a, T b) ;

	/**
	 * addition with the modification to return top for sums that are larger than top
	 **/
	template<class T, T top> static T bplus (T a, T b) ;
	template<class T> static T bplus (T a, T b, T top) ;

	/**
	 * calculate the sum s[0]+...s[n-1] modulo m (needs =, +, % for T and =, CTYPE(), <, ++ for CTYPE)
	 **/
	template<class T, class CTYPE> static T modsum (T* s, CTYPE n, T m) ;

	/**
	 * round up x to nearest integer
	 **/
	template<class IT, class FT> static IT roundup (FT x) ;

	/**
	 * compute 2-logarithm of n (rounded up to nearest int), i.e. number of bits needed to store values from {0,...,n-1}
	 **/
	template<class T> static T log2_ceil (T n) ;
} ;

template<class T>
T AUtils::max (T a, T b)
{
	if (a > b) {
		return a ;
	}
	else {
		return b ;
	}
}

template<class T>
T AUtils::min (T a, T b)
{
	if (a < b) {
		return a ;
	}
	else {
		return b ;
	}
}

template<class T>
T AUtils::div_roundup (T a, T b)
{
	T c = b-- ;
	return ((a + b) / c) ;
}

template<class T>
T AUtils::bminus (T a, T b)
{
	if (a > b) {
		return (a - b) ;
	}
	else {
		return T() ;
	}
}

template<class T, T top>
T AUtils::bplus (T a, T b)
{
	a += b ;
	if (a > top) {
		return top ;
	}
	else {
		return a ;
	}
}

template<class T>
T AUtils::bplus (T a, T b, T top)
{
	a += b ;
	if (a > top) {
		return top ;
	}
	else {
		return a ;
	}
}

template<class T, class CTYPE>
T AUtils::modsum (T* s, CTYPE n, T m)
{
	T retval = 0 ;
	for (CTYPE i = CTYPE() ; i < n ; ++i) {
		retval = (retval + s[i]) % m ;
	}
	return retval ;
}

template<class IT, class FT>
IT AUtils::roundup (FT x)
{
	IT retval = 0 ;
	FT intpart = (FT) ((IT) x) ;
	if (x - intpart == (FT) 0.0) {
		retval = (IT) x ;
	}
	else {
		retval = ((IT) x) + 1 ;
	}
	return retval ;
}

template<class T>
T AUtils::log2_ceil (T n)
{
	T retval = 0 ;
	while (n > 1) {
		n = div_roundup<T> (n, 2) ;
		++retval ;
	}
	return retval ;
}

#endif // ndef SH_AUTILS_H

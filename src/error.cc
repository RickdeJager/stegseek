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

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "BinaryIO.h"
#include "common.h"
#include "error.h"

//
// class ArgError
//
ArgError::ArgError (const char* msgfmt, ...)
	: SteghideError()
{
	va_list ap ;
	va_start (ap, msgfmt) ,
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void ArgError::printMessage () const
{
	SteghideError::printMessage() ;
	std::cerr << "steghide: " << _("type \"steghide --help\" for help.") << std::endl ;
}

//
// class BinaryInputError
//
BinaryInputError::BinaryInputError (std::string fn, FILE* s)
	: SteghideError()
{
	if (feof (s)) {
		if (fn == "") {
			setMessage(_("premature end of data from standard input.")) ;
			setType (STDIN_EOF) ;
		}
		else {
			setMessage(compose (_("premature end of file \"%s\"."), fn.c_str())) ;
			setType (FILE_EOF) ;
		}
	}
	else {
		if (fn == "") {
			setMessage(_("an error occured while reading data from standard input.")) ;
			setType (STDIN_ERR) ;
		}
		else {
			setMessage(compose (_("an error occured while reading data from the file \"%s\"."), fn.c_str())) ;
			setType (FILE_ERR) ;
		}
	}
}

BinaryInputError::TYPE BinaryInputError::getType (void)
{
	return type ;
}

void BinaryInputError::setType (BinaryInputError::TYPE t)
{
	type = t ;
}

//
// class BinaryOutputError
//
BinaryOutputError::BinaryOutputError (std::string fn)
	: SteghideError()
{
	if (fn == "") {
		setMessage(_("an error occured while writing data to standard output.")) ;
		setType(STDOUT_ERR) ;
	}
	else {
		setMessage(compose (_("an error occured while writing data to the file \"%s\"."), fn.c_str())) ;
		setType(FILE_ERR) ;
	}
}

BinaryOutputError::TYPE BinaryOutputError::getType (void)
{
	return type ;
}

void BinaryOutputError::setType (BinaryOutputError::TYPE t)
{
	type = t ;
}

//
// class UnSupFileFormat
//
UnSupFileFormat::UnSupFileFormat (BinaryIO *io)
	: SteghideError()
{
	if (io->is_std()) {
		setMessage(_("the file format of the data from standard input is not supported.")) ;
	}
	else {
		setMessage(compose (_("the file format of the file \"%s\" is not supported."), io->getName().c_str())) ;
	}
}

//
// class NotImplementedError
//
NotImplementedError::NotImplementedError (const char *msgfmt, ...)
	: SteghideError()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void NotImplementedError::printMessage () const
{
	SteghideError::printMessage() ;
	printf (_("This feature is not (yet) available. Please let me (shetzl@chello.at) know\n"
		"that you want to use this functionality to increase the chance that this will\n"
		"be implemented in the near future. Steghide has to exit now. Sorry.\n")) ;
}

//
// class CorruptDataError
//
CorruptDataError::CorruptDataError (const char* msgfmt, ...)
	: SteghideError()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void CorruptDataError::printMessage () const
{
	SteghideError::printMessage() ;
	printf (_("Other possible reasons for this error are that the passphrase is wrong\n"
		"or that there is no embedded data.\n")) ;
}

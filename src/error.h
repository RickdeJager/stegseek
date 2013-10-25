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

#ifndef SH_ERROR_H
#define SH_ERROR_H

#include <cstdio>
#include <string>

#include "SteghideError.h"

class BinaryIO ;

class ArgError : public SteghideError {
	public:
	ArgError (const char *msgfmt, ...) ;

	void printMessage (void) const ;
} ;

class BinaryInputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, FILE_EOF, STDIN_ERR, STDIN_EOF } ;

	BinaryInputError (std::string fn, FILE* s) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

class BinaryOutputError : public SteghideError {
	public:
	enum TYPE { FILE_ERR, STDOUT_ERR } ;

	BinaryOutputError (std::string fn) ;

	TYPE getType (void) ;

	protected:
	void setType (TYPE t) ;

	private:
	TYPE type ;
} ;

class UnSupFileFormat : public SteghideError {
	public:
	UnSupFileFormat (BinaryIO *io) ;
} ;

class NotImplementedError : public SteghideError {
	public:
	NotImplementedError (const char* msgfmt, ...) ;

	void printMessage (void) const ;
} ;

/**
 * \class CorruptDataError
 * \brief is thrown as exception when corrupt data is encountered during extraction
 *
 * A possible cause of this exception being thrown is a wrong password.
 **/
class CorruptDataError : public SteghideError {
	public:
	CorruptDataError (const char* msgfmt, ...) ;

	void printMessage (void) const ;
} ;

#endif

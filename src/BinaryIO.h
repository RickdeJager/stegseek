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

#ifndef SH_BINARYIO_H
#define SH_BINARYIO_H

#include <cstdio>
#include <string>

#include "common.h"

/**
 * \class BinaryIO
 * \brief provides methods for file i/o as needed by the rest of steghide
 **/
class BinaryIO {
	public:
	enum MODE { READ, WRITE } ;

	BinaryIO (void) ;

	/**
	 * construct a BinaryIO object
	 * \param fn the filename ("" to indicate stdin/stdout)
	 * \param m the mode (BinaryIO::READ or BinaryIO::WRITE)
	 *
	 * The file described by fn is opened in the given mode.
	 **/
	BinaryIO (const std::string& fn, MODE m) ;

	~BinaryIO (void) ;

	/**
	 * get the name (with path) of this file
	 **/
	const std::string& getName (void) const
		{ return Name ; } ;

	/**
	 * is this file currently opened ?
	 **/
	bool is_open (void) const
		{ return FileOpen ; } ;

	/**
	 * is this file a standard stream (stdin or stdout) ?
	 **/
	bool is_std (void) const
		{ return (getStream() != NULL && getName() == "") ; } ;

	/**
	 * get the current position in the current file
	 **/
	unsigned long getPos (void) const
		{ return ftell(getStream()) ; } ;

	/**
	 * is the current state of this file at the end of the file
	 **/
	bool eof (void) const ;

	/**
	 * open the file given by fn in the mode m
	 * \param fn a filename ("" to indicate stdin/stdout)
	 * \param m the mode (BinaryIO::READ or BinaryIO::WRITE)
	 **/
	void open (const std::string& fn, MODE m) ;	

	/**
	 * close the currently open file - it is save to call close() even if is_std() is true
	 **/
	void close (void) ;

	/**
	 * read one byte from the file
	 **/
	BYTE read8 (void) ;

	/**
	 * read two bytes from the file using little-endian byte ordering
	 **/
	UWORD16 read16_le (void) ;

	/**
	 * read two bytes from the file using big-endian byte ordering
	 **/
	UWORD16 read16_be (void) ;

	/**
	 * read four bytes from the file using little-endian byte ordering
	 **/
	UWORD32 read32_le (void) ;

	/**
	 * read four bytes from the file using big-endian byte ordering
	 **/
	UWORD32 read32_be (void) ;

	/**
	 * read n bytes (little endian byte ordering)
	 * \param n the number of bytes to read (must be <= 4)
	 **/
	UWORD32 read_le (unsigned short n) ;

	/**
	 * read a string with length len from the file
	 **/
	std::string readstring (unsigned int len) ;

	/**
	 * write one byte to the file
	 **/
	void write8 (BYTE val) ;

	/**
	 * write two bytes to the file using little-endian byte ordering
	 **/
	void write16_le (UWORD16 val) ;

	/**
	 * write two bytes to the file using big-endian byte ordering
	 **/
	void write16_be (UWORD16 val) ;

	/**
	 * write four bytes to the file using little-endian byte ordering
	 **/
	void write32_le (UWORD32 val) ;

	/**
	 * write four bytes to the file using big-endian byte ordering
	 **/
	void write32_be (UWORD32 val) ;

	/**
	 * write n bytes of val (little endian byte ordering)
	 * \param n the number of bytes to write (must be <= 4)
	 * \param val the value
	 **/
	void write_le (UWORD32 val, unsigned short n) ;

	void writestring (const std::string& s) ;

	/**
	 * get the underlying cstdio FILE* pointer
	 **/
	FILE* getStream (void) const
		{ return Stream ; } ;

	protected:
	void setStream (FILE* s)
		{ Stream = s ; } ;

	void setName (const std::string& fn)
		{ Name = fn ; } ;

	MODE getMode (void) const
		{ return Mode ; } ;

	void setMode (MODE m)
		{ Mode = m ; } ;

	private:
	std::string Name ;
	FILE *Stream ;
	bool FileOpen ;
	MODE Mode ;

	void init (void) ;

	void set_open (bool o)
		{ FileOpen = o ; } ;

	/**
	 * when opening a file in write mode perform various checks depending on the value of the force argument
	 **/
	void checkForce (const std::string& fn) const ;

	/**
	 * check if the file described by fn exists
	 * \return true iff a fopen call with fn as file name succeeded
	 **/
	bool Fileexists (const std::string& fn) const ;
} ;

#endif /* ndef SH_BINARYIO_H */

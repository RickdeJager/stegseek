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

#ifndef SH_MSG_H
#define SH_MSG_H

#include <cstdarg>
#include <string>

class MessageBase {
	public:
	MessageBase (void) ;
	MessageBase (std::string msg) ;
	MessageBase (const char* msgfmt, ...) ;
	virtual ~MessageBase() {} ;

	const std::string& getMessage (void) const
		{ return Message ; } ;

	void setMessage (std::string msg)
		{ Message = msg ; } ;

	/**
	 * toggle newline printing on/off
	 * \param prnl wether to print a newline character after the message
	 **/
	void setNewline (bool prnl)
		{ Newline = prnl ; } ;

	/**
	 * return either "\n" or "" depending on wether this message should be followed by a newline or not
	 **/
	const std::string getNewline (void) const
		{ return (Newline ? "\n" : "") ; } ;

	void setMessage (const char *msgfmt, ...) ;
	virtual void printMessage (void) const = 0 ;

	protected:
	static const unsigned int MsgMaxSize = 512 ;

	std::string compose (const char *msgfmt, ...) const ;
	std::string vcompose (const char *msgfmt, va_list ap) const ;

	private:
	std::string Message ;
	bool Newline ;
} ;

class Message : public MessageBase {
	public:
	Message (void) : MessageBase() {} ;
	Message (std::string msg) : MessageBase (msg) {} ;
	Message (const char *msgfmt, ...) ;

	void printMessage (void) const ;
} ;

class VerboseMessage : public MessageBase {
	public:
	VerboseMessage (void) : MessageBase() {} ;
	VerboseMessage (std::string msg) : MessageBase (msg) {} ;
	VerboseMessage (const char *msgfmt, ...) ;

	void printMessage (void) const ;
} ;

class Warning : public MessageBase {
	public:
	Warning (void) : MessageBase() {} ;
	Warning (std::string msg) : MessageBase (msg) {} ;
	Warning (const char *msgfmt, ...) ;

	void printMessage (void) const ;
} ;

class CriticalWarning : public MessageBase {
	public:
	CriticalWarning (void) : MessageBase() {} ;
	CriticalWarning (std::string msg) : MessageBase (msg) {} ;
	CriticalWarning (const char *msgfmt, ...) ;

	void printMessage (void) const ;
} ;

class Question : public MessageBase {
	public:
	Question (void) ;
	Question (std::string msg) ;
	Question (const char *msgfmt, ...) ;

	void printMessage (void) const ;

	/**
	 * wait for the user to answer the question (should be printed before)
	 * \return true iff the user answers with yes, i.e. presses the yeschar-key
	 **/
	bool getAnswer (void) ;

	private:
	std::string yeschar ;
	std::string nochar ;
} ;

#endif	/* ndef SH_MSG_H */

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

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>

#include "common.h"
#include "msg.h"
#include "Terminal.h"

//
// class MessageBase
//
MessageBase::MessageBase ()
{
	setMessage (std::string(_("__no_message_defined__"))) ;
	setNewline (true) ;
}

MessageBase::MessageBase (std::string msg)
{
	setMessage (msg) ;
	setNewline (true) ;
}

MessageBase::MessageBase (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (msgfmt, ap) ;
	va_end (ap) ;
	setNewline (true) ;
}

void MessageBase::setMessage (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

std::string MessageBase::compose (const char *msgfmt, ...) const
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	std::string retval = vcompose (msgfmt, ap) ;
	va_end (ap) ;
	return retval ;
}

std::string MessageBase::vcompose (const char *msgfmt, va_list ap) const
{
	char *str = new char[MsgMaxSize] ;
	vsnprintf (str, MsgMaxSize, msgfmt, ap) ;
	std::string retval (str) ;
	delete[] str ;
	return retval ;
}

//
// class Message
//
Message::Message (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Message::printMessage () const
{
	if (Args.Verbosity.getValue() == NORMAL ||
		Args.Verbosity.getValue() == VERBOSE) {
		std::cerr << getMessage() << getNewline() ;
	}
}

//
// class VerboseMessage
//
VerboseMessage::VerboseMessage (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
	setNewline (true) ;
}

void VerboseMessage::printMessage () const
{
	if (Args.Verbosity.getValue() == VERBOSE) {
		std::cerr << getMessage() << getNewline() ;
	}
}

//
// class Warning
//
Warning::Warning (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Warning::printMessage () const
{
	if (Args.Verbosity.getValue() != QUIET) {
		std::cerr << "steghide: " << _("warning:") << " " << getMessage() << getNewline() ;
	}
}

//
// class CriticalWarning
//
CriticalWarning::CriticalWarning (const char *msgfmt, ...)
	: MessageBase()
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void CriticalWarning::printMessage () const
{
	std::cerr << "steghide: " << _("warning:") << " " << getMessage() << getNewline() ;
}

//
// class Question
//
Question::Question (void)
	: MessageBase()
{
	yeschar = std::string (_("y")) ;
	nochar = std::string (_("n")) ;
}

Question::Question (std::string msg)
	: MessageBase (msg)
{
	yeschar = std::string (_("y")) ;
	nochar = std::string (_("n")) ;
}

Question::Question (const char *msgfmt, ...)
	: MessageBase()
{
	yeschar = std::string (_("y")) ;
	nochar = std::string (_("n")) ;

	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

void Question::printMessage () const
{
#ifndef HAVE_TERMIOS_H
	Warning w (_("unknown terminal. you might need to press <Enter> after answering.")) ;
	w.printMessage() ;
#endif
	std::cerr << getMessage() << " (" << yeschar << "/" << nochar << ") " ;
}

bool Question::getAnswer ()
{
	Terminal term ;
	term.SingleKeyOn() ;
	char input[2] ;
	input[0] = std::cin.get() ;
	input[1] = '\0' ;
	bool retval = (std::string (input) == yeschar) ;
	term.reset() ;
	std::cerr << std::endl ;

	return retval ;
}

//
// debugging output
//
#ifdef DEBUG
void printDebug (unsigned short level, const char *msgfmt, ...)
{
	if (RUNDEBUGLEVEL(level)) {
		va_list ap ;
		va_start (ap, msgfmt) ;
		vfprintf (stderr, msgfmt, ap) ;
		va_end (ap) ;
		fprintf (stderr, "\n") ;
	}
}
#endif

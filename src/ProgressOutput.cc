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

#include <cmath>

#include "ProgressOutput.h"
#include "common.h"

ProgressOutput::ProgressOutput ()
	: Message("__nomessage__")
{
	LastUpdate = time(NULL) - 1 ; // -1 to ensure that message is written first time
}

ProgressOutput::ProgressOutput (const std::string& m)
	: Message(m)
{
	LastUpdate = time(NULL) - 1 ; // -1 to ensure that message is written first time
}

void ProgressOutput::setMessage (const char *msgfmt, ...)
{
	va_list ap ;
	va_start (ap, msgfmt) ;
	setMessage (vcompose (msgfmt, ap)) ;
	va_end (ap) ;
}

std::string ProgressOutput::vcompose (const char *msgfmt, va_list ap) const
{
	char *str = new char[200] ;
	vsnprintf (str, 200, msgfmt, ap) ;
	std::string retval (str) ;
	delete[] str ;
	return retval ;
}

void ProgressOutput::update (float rate)
{
	time_t now = time(NULL) ;
	if (LastUpdate < now) {
		LastUpdate = now ;
		printf ("\r%s %.1f%%", Message.c_str(), 100.0 * rate) ;
		fflush (stdout) ;
	}
}

void ProgressOutput::done (float rate, float avgweight) const
{
	printf ("\r%s %.1f%%", Message.c_str(), 100.0 * rate) ;
	if (avgweight != NoAvgWeight) {
		printf (" (%.1f)", avgweight) ;
	}
	printf (_(" done")) ;
	printf ("    \n") ; // be sure to overwrite old line (even in a language with one-letter done)
	fflush (stdout) ;
}

void ProgressOutput::done () const
{
	printf ("\r%s", Message.c_str()) ;
	printf (_(" done\n")) ;
}

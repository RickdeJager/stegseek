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

#ifndef SH_PROGRESSOUTPUT_H
#define SH_PROGRESSOUTPUT_H

#include <cstdarg>
#include <ctime>
#include <string>

/**
 * \class ProgressOutput
 * \brief prints the progress to stdout
 **/
class ProgressOutput {
	public:
	/**
	 * create an empty ProgressOutput object
	 **/
	ProgressOutput (void) ;

	/**
	 * create a ProgressOutput object
	 * \param m the message to be displayed
	 **/
	ProgressOutput (const std::string& m) ;

	void setMessage (const std::string& m)
		{ Message = m ; } ;

	void setMessage (const char *msgfmt, ...) ;

	/**
	 * update the output (taking update frequency into account) with rate as percentage
	 **/
	void update (float rate) ;

	/**
	 * update the output appending "done" and a newline (no rate nor average weight)
	 **/
	void done (void) const ;

	/**
	 * update the output appending rate, [average edge weight], "done" and a newline
	 * \param rate the rate of matched vertices
	 * \param avgweight the average edge weight (is not printed if not given)
	 **/
	void done (float rate, float avgweight = NoAvgWeight) const ;

	static const float NoAvgWeight = -1.0 ;

	protected:
	std::string vcompose (const char *msgfmt, va_list ap) const ;

	private:
	std::string Message ;
	time_t LastUpdate ;
} ;

#endif // ndef SH_PROGRESSOUTPUT_H

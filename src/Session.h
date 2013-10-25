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

#ifndef SH_SESSION_H
#define SH_SESSION_H

#include <map>
#include <string>

#include "common.h"

class Session {
	public:
	Session (void) {} ;

	void run (void) ;

	private:
	std::string stripDir (std::string s) const ;
	void printInfo (void) ;
	void printEncInfo (void) ;
	void printVersion (void) ;
	void printHelp (void) ;
	void printLicense (void) ;
#ifdef DEBUG
	void printFrequencies (void) ;
	/**
	 * add the frequencies in f2 to those in f1 (changing only f1 - like f1 += f2)
	 **/
	void addFrequencies (std::map<SampleKey,unsigned long>* f1, const std::map<SampleKey,unsigned long>* f2) ;
#endif
} ;
#endif //ndef SH_SESSION_H

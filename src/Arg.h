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

#ifndef SH_ARG_H
#define SH_ARG_H

#include <list>
#include <string>

#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"

// to include DEBUG if defined
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

template<class T> class Arg {
	public:
	Arg (void) : set(false) {} ;
	Arg (T v, bool setbyuser = true) ;

	T getValue (void) const
		{ return Value ; } ;

	void setValue (T v, bool setbyuser = true) ;

	bool is_set (void) const
		{ return set ; } ;

	private:
	T Value ;
	bool set ;
} ;

enum COMMAND { EMBED, EXTRACT, INFO, ENCINFO, SHOWVERSION, SHOWLICENSE, SHOWHELP, PRINTFREQS } ;
enum VERBOSITY { QUIET, NORMAL, VERBOSE, STATS } ;
enum DEBUGCOMMAND { NONE, PRINTGRAPH, PRINTGMLGRAPH, PRINTGMLVERTEX } ;

typedef Arg<COMMAND> ArgCommand ;
typedef Arg<VERBOSITY> ArgVerbosity ;
typedef Arg<bool> ArgBool ;
typedef Arg<std::string> ArgString ;
typedef Arg<unsigned long> ArgULong ;
typedef Arg<int> ArgInt ;
typedef Arg<float> ArgFloat ;
typedef Arg<EncryptionAlgorithm> ArgEncAlgo ;
typedef Arg<EncryptionMode> ArgEncMode ;
typedef Arg<unsigned int> ArgUInt ;
typedef Arg<std::list<std::string> > ArgStringList ;
typedef Arg<DEBUGCOMMAND> ArgDebugCommand ;

#endif // ndef SH_ARG_H

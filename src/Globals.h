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

#ifndef SH_GLOBALS_H
#define SH_GLOBALS_H

#include <cstdlib>

class CvrStgFile ;
class Graph ;

/**
 * \class Globals
 * \brief some useful pointers that should be global
 *
 * This class provides some useful global variables. They are not static,
 * instead there exists a global Globs object to make it easy to use
 * different Globals objects during one execution (this is necessary for
 * some unit-tests).
 *
 * The Graph constructor as well as the CvrStgFile constructor write itself
 * into the Globs object. Doing this so early is necessary because the
 * construction of a Graph or CvrStgFile object might already need a correctly
 * set Globs object.
 *
 * During one "normal" (i.e. non-unit-test) execution of steghide only one
 * Globals object will be used, namely the one created in main(), filled in
 * the Graph and the CvrStgFile constructor and stored at the Globs pointer.
 *
 * The main purpose of making these variables global is to save memory in
 * classes that are small but used in large numbers (e.g. *SampleValue,...).
 * Using static pointers in these classed would be too chaotic to reset for
 * the unit tests and non-static pointers would need too much memory.
 **/
class Globals {
	public:
	Globals (CvrStgFile* f = NULL, Graph* g = NULL)
		: TheCvrStgFile(f), TheGraph(g) {} ;

	/// the cover-/stego- file that is operated on (set in CvrStgFile::CvrStgFile)
	CvrStgFile* TheCvrStgFile ;

	/// the graph that is built upon the cover-/stego-file (set in Graph::Graph)
	Graph* TheGraph ;

	void operator= (const Globals& g)
	{
		TheCvrStgFile = g.TheCvrStgFile ; 
		TheGraph = g.TheGraph ;
	}

	void reset (void)
	{
		TheCvrStgFile = NULL ;
		TheGraph = NULL ;
	}
} ;

#endif // ndef SH_GLOBALS_H

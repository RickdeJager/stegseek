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

#ifndef SH_EMBEDDER_H
#define SH_EMBEDDER_H

#include "BitString.h"
class Edge ;
class CvrStgFile ;
class Graph ;
class Matching ;
class ProgressOutput ;
class Vertex ;

class Embedder {
	public:
	Embedder (void) ;

	~Embedder (void) ;

	void embed (void) ;

	private:
	static const unsigned int Default_NConstrHeur = 1 ;

	BitString ToEmbed ;

	/**
	 * do the matching algorithms
	 **/
	const Matching* calculateMatching (ProgressOutput* prout) ;

	/**
	 * embed the two bits represented by the two vertices adjacent to e
	 **/
	void embedEdge (Edge *e) ;

	/**
	 * embed the bit represented by the Vertex v
	 **/
	void embedExposedVertex (Vertex *v) ;
} ;

#endif // ndef SH_EMBEDDER_H

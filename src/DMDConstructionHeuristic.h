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

#ifndef SH_DMDCONSTRUCTIONHEURISTIC_H
#define SH_DMDCONSTRUCTIONHEURISTIC_H

#include "MatchingAlgorithm.h"
class Vertex ;
#include "common.h"

/**
 * \class DMDConstructionHeuristic
 * \brief an implementation of the "dynamic minimum degree" heuristic for contruction a matching
 **/
class DMDConstructionHeuristic : public MatchingAlgorithm {
	public:
	DMDConstructionHeuristic (Graph* g, Matching* m, float goal = 100.0) ;

	virtual ~DMDConstructionHeuristic (void) {} ;

	const char* getName (void) const
		{ return "Dynamic Minimum Degree Construction Heuristic" ; } ;

	void run (void) ;

	private:
	static const VertexLabel MinDegNotFound = VERTEXLABEL_MAX ;

	std::vector<Vertex*> AvailableVertices ;

	VertexLabel findMinDegIndex (const std::vector<Vertex*>& vertices) ;
} ;

#endif // ndef SH_DMDCONSTRUCTIONHEURISTIC_H

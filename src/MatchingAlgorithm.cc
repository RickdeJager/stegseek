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

#include "Graph.h"
#include "MatchingAlgorithm.h"

MatchingAlgorithm::MatchingAlgorithm (Graph* g, Matching* m, float goal)
	: TheGraph(g), TheMatching(m)
{
	setGoal(goal) ;
}

void MatchingAlgorithm::setGoal (float goal)
{
	// if goal is 100.0 and the graph has an odd number of vertices the algorithm won't spend time
	// searching an edge for the last vertex as / rounds down to the next smaller integer
	CardinalityGoal = (unsigned long) (((float) TheGraph->getNumVertices()) * (goal / 100.0)) / 2 ;
}

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

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

#include "BmpRGBSampleValue.h"
#include "CvrStgFile.h"
#include "Graph.h"
#include "SampleValue.h"
#include "SampleValueAdjacencyList.h"
#include "WavPCMSampleValue.h"
#include "common.h"

SampleValueAdjacencyList::SampleValueAdjacencyList (SampleValueLabel numsvs)
{
	AdjacencyList = std::vector<std::vector<SampleValue*> > (numsvs) ;
}

bool SampleValueAdjacencyList::operator== (const SampleValueAdjacencyList& sval)
{
	bool equal = false ;
	if (AdjacencyList.size() == sval.AdjacencyList.size()) {
		equal = true ;
		for (unsigned long i = 0 ; i < AdjacencyList.size() ; i++) {
			bool rowequal = false ;
			if (AdjacencyList[i].size() == sval.AdjacencyList[i].size()) {
				std::set<SampleValue*, SampleValuesLess> thisset (AdjacencyList[i].begin(), AdjacencyList[i].end()) ;
				std::set<SampleValue*, SampleValuesLess>::const_iterator thisit = thisset.begin() ;
				std::set<SampleValue*, SampleValuesLess> otherset (sval.AdjacencyList[i].begin(), sval.AdjacencyList[i].end()) ;
				std::set<SampleValue*, SampleValuesLess>::const_iterator otherit = otherset.begin() ;
				rowequal = true ;
				for ( ; thisit != thisset.end() && otherit != otherset.end() ; thisit++, otherit++) {
					rowequal = (**thisit == **otherit) ;
				}
			}

			equal = rowequal && equal ;
		}
	}
	return equal ;
}

void SampleValueAdjacencyList::sort ()
{
	for (SampleValueLabel lbl = 0 ; lbl < AdjacencyList.size() ; lbl++) {
		std::vector<SampleValue*>& row = AdjacencyList[lbl] ;

		if (row.size() > 0) {
			UWORD32* distances = new UWORD32[row.size() + 1] ;
			for (unsigned int i = 0 ; i < row.size() ; i++) {
				distances[i] = Globs.TheGraph->SampleValues[lbl]->calcDistance (row[i]) ;
			}

			quicksort (row, distances, 0, row.size() - 1) ;

			delete[] distances ;
		}
	}
}

void SampleValueAdjacencyList::quicksort (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r)
{
	if (l < r) {
		unsigned int p = partition (oppneighs, distances, l, r, distances[r]) ;
		if (p > 0) {
			quicksort (oppneighs, distances, l, p - 1) ;
		}
		quicksort (oppneighs, distances, p + 1, r) ;
	}
}

unsigned int SampleValueAdjacencyList::partition (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int l, unsigned int r, UWORD32 x)
{
	unsigned int i = l, j = r ;
	while (i < j) {
		while ((distances[i] < x) && (i < r)) {
			i++ ;
		}
		while ((distances[j] >= x) && (j > l)) {
			j-- ;
		}
		if (i < j) {
			swap (oppneighs, distances, i, j) ;
		}
	}
	swap (oppneighs, distances, i, r) ;
	return i ;
}

void SampleValueAdjacencyList::swap (std::vector<SampleValue*>& oppneighs, UWORD32* distances, unsigned int i, unsigned int j)
{
	SampleValue* tmp1 = oppneighs[i] ;
	UWORD32 tmp2 = distances[i] ;
	oppneighs[i] = oppneighs[j] ;
	distances[i] = distances[j] ;
	oppneighs[j] = tmp1 ;
	distances[j] = tmp2 ;
}

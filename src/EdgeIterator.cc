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

#include "Edge.h"
#include "EdgeIterator.h"
#include "Graph.h"
#include "SampleOccurence.h"
#include "Vertex.h"
#include "common.h"

EdgeIterator::EdgeIterator ()
	: SVALIndices(NULL)
{
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
}

EdgeIterator::EdgeIterator (Vertex *v, ITERATIONMODE m)
{
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	reset(v, m) ;
}

EdgeIterator::EdgeIterator (const EdgeIterator& eit)
{
	CurrentEdge = eit.CurrentEdge ;
	Mode = eit.Mode ;
	SVALIndices = new unsigned long[Globs.TheCvrStgFile->getSamplesPerVertex()] ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SVALIndices[i] = eit.SVALIndices[i] ;
	}
	Finished = eit.Finished ;
	SampleOccurenceIt = eit.SampleOccurenceIt ;
}

EdgeIterator::~EdgeIterator ()
{
	delete[] SVALIndices ;
}

void EdgeIterator::operator++ ()
{
	myassert (!Finished) ;

	Vertex* srcv = CurrentEdge.getVertex1() ;
	unsigned short srcidx = CurrentEdge.getIndex1() ;

	switch (Mode) {
		case SAMPLEOCCURENCE:
		{
			SampleValue* srcsv = srcv->getSampleValue(srcidx) ;
			SampleValue* destsv = (*(Globs.TheGraph->SVALists[srcv->getTargetValue(srcidx)]))[srcsv][SVALIndices[srcidx]] ;

			bool cont = false ;
			do {
				SampleOccurenceIt++ ;

				cont = false ;
				if (SampleOccurenceIt != Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
					if ((srcsv->getEmbeddedValue() != SampleOccurenceIt->getVertex()->getTargetValue(SampleOccurenceIt->getIndex())) || (SampleOccurenceIt->getVertex()->getLabel() == srcv->getLabel())) {
						cont = true ;
					}
				}
			} while (cont) ;

			if (SampleOccurenceIt == Globs.TheGraph->SampleOccurences[destsv->getLabel()].end()) {
				// search new destination sample value
				SVALIndices[srcidx]++ ;
				findNextEdge() ;
			}
			break ;
		}
		case SAMPLEVALUE:
		{
			SVALIndices[srcidx]++ ;
			findNextEdge() ;
			break ;
		}
	}

	// increment EdgeIndex while checking that it has not become too high
	if (++EdgeIndex >= MaxNumEdges) {
		Finished = true ;
	}

	if (!Finished) {
		CurrentEdge.set2 (SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}
}

void EdgeIterator::reset (ITERATIONMODE m)
{
	Mode = m ;
	Finished = false ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SVALIndices[i] = 0 ;
	}
	findNextEdge() ;
	EdgeIndex = 0 ;
	if (!Finished) {
		CurrentEdge.set2 (SampleOccurenceIt->getVertex(), SampleOccurenceIt->getIndex()) ;
	}
}

void EdgeIterator::reset (Vertex* v, ITERATIONMODE m)
{
	CurrentEdge.setVertex1 (v) ;
	reset(m) ;
}

void EdgeIterator::findNextEdge ()
{
	UWORD32 mindist = UWORD32_MAX ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		Vertex* srcv = CurrentEdge.getVertex1() ;
		SampleValue* srcsv = srcv->getSampleValue(i) ;
		SampleValue* destsv = NULL ;
		std::list<SampleOccurence>::const_iterator soccit_candidate ;

		// increment SVALIndices[i] until it points to a valid destination sample value
		while (SVALIndices[i] < (*(Globs.TheGraph->SVALists[srcv->getTargetValue(i)]))[srcsv].size()) {
			destsv = (*(Globs.TheGraph->SVALists[srcv->getTargetValue(i)]))[srcsv][SVALIndices[i]] ;

			// look for a sample occurence of destsv - thereby setting soccit_candidate
			bool found = false ;
			const std::list<SampleOccurence>& socc = Globs.TheGraph->SampleOccurences[destsv->getLabel()] ;
			soccit_candidate = socc.begin() ;
			while (!found && soccit_candidate != socc.end()) {
				if ((soccit_candidate->getVertex()->getLabel() == srcv->getLabel()) || 
					(srcsv->getEmbeddedValue() != soccit_candidate->getVertex()->getTargetValue(soccit_candidate->getIndex()))) {
					soccit_candidate++ ;
				}
				else {
					found = true ;
				}
			}

			if (found) {
				break ;
			}
			else {
				SVALIndices[i]++ ;
			}
		}

		// test if the destination sample value leads to edge with (until now) minimal distance - thereby setting CurrentEdge.Index1 and SampleOccurenceIt
		if (SVALIndices[i] < (*(Globs.TheGraph->SVALists[srcv->getTargetValue(i)]))[srcsv].size()) {
			UWORD32 thisdist = srcsv->calcDistance(destsv) ;
			if (thisdist < mindist) {
				mindist = thisdist ;
				CurrentEdge.setIndex1(i) ;
				SampleOccurenceIt = soccit_candidate ;
			}
		}
	}

	if (mindist == UWORD32_MAX) {
		// no edge has been found
		Finished = true ;
	}
}

#ifdef DEBUG
void EdgeIterator::print (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "Current Edge:" << std::endl ;
	CurrentEdge.print(spc + 1) ;
	std::cerr << space << "SampleOccurenceIt: <" << SampleOccurenceIt->getVertex()->getLabel() << "," << SampleOccurenceIt->getIndex() << ">" << std::endl ;
	std::cerr << space << "Finished: " << Finished << std::endl ;
	std::cerr << space << "EdgeIndex: " << EdgeIndex << std::endl ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		std::cerr << space << "SVALIndices[" << i << "]: " << SVALIndices[i] << std::endl ;
	}
}
#endif

UWORD32 EdgeIterator::MaxNumEdges = UWORD32_MAX ;

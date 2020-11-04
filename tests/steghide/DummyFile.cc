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

#include "BitString.h"
#include "Selector.h"

#include "DummyFile.h"
#include "DummySampleValue.h"

DummyFile::DummyFile (UWORD16 s, std::vector<std::vector<bool> >* svam)
	: SampleValueAdjacencyMatrix(svam)
{
	Samples.resize (s) ;
	for (UWORD16 i = 0 ; i < s ; i++) {
		Samples[i] = i ;
	}
	setSamplesPerVertex (SamplesPerVertex) ;
	setEmbValueModulus (EmbValueModulus) ;
	setRadius (0) ; // undefined, neighbourhood relatin decided by svam
}

std::list<CvrStgFile::Property> DummyFile::getProperties () const
{
	std::list<CvrStgFile::Property> retval ;
	retval.push_back (CvrStgFile::Property (_("format"), "dummy")) ;
	return retval ;
}

unsigned long DummyFile::getNumSamples () const
{
	return Samples.size() ;
}

SampleValue* DummyFile::getSampleValue (const SamplePos pos) const
{
	return (SampleValue*) (new DummySampleValue (Samples[pos])) ;
}

void DummyFile::replaceSample (const SamplePos pos, const SampleValue* s)
{
	myassert (pos < Samples.size()) ;
	DummySampleValue *sample = (DummySampleValue*) s ;
	Samples[pos] = sample->getValue() ;
}

void DummyFile::createGraph (std::vector<std::list<UWORD16> >& adjlist, BitString** bs, CvrStgFile** f, Selector** s)
{
	unsigned int numvertices = adjlist.size() ;

	// eliminate duplicated edges from adjlist (only allow v_i -> ...,v_j,... with j > i)
	for (UWORD16 srcv = 0 ; srcv < numvertices ; srcv++) {
		for (std::list<UWORD16>::iterator destv = adjlist[srcv].begin() ; destv != adjlist[srcv].end() ; destv++) {
			if (srcv > (*destv)) {
				adjlist[srcv].erase (destv) ;
			}
		}
	}

	// allocate memory for SampleValue Adjacency-Matrix
	std::vector<std::vector<bool> >* svam = new std::vector<std::vector<bool> > (2 * numvertices) ;
	for (UWORD16 srcv = 0 ; srcv < numvertices ; srcv++) {
		(*svam)[2*srcv] = std::vector<bool> (2 * numvertices) ;
		(*svam)[2*srcv + 1] = std::vector<bool> (2 * numvertices) ;
	}

	// fill SampleValue Adjacency-Matrix
	for (UWORD16 srcv = 0 ; srcv < numvertices ; srcv++) {
		UWORD16 srcsv = 2 * srcv ;
		for (std::list<UWORD16>::const_iterator destv = adjlist[srcv].begin() ; destv != adjlist[srcv].end() ; destv++) {
			UWORD16 destsv = 2 * (*destv) + 1 ;
			(*svam)[srcsv][destsv] = true ;
			(*svam)[destsv][srcsv] = true ;
		}
	}

	(*f) = (CvrStgFile*) new DummyFile (2 * numvertices, svam) ;
	(*bs) = new BitString ((unsigned long) numvertices) ; // zeros only
	(*s) = new Selector (2 * numvertices) ; // identity permutation
}

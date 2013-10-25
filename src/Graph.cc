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

#include <ctime>
#include <list>
#include <map>
#include <vector>

#include "BitString.h"
#include "CvrStgFile.h"
#include "Edge.h"
#include "Graph.h"
#include "Selector.h"
#include "Vertex.h"
#include "common.h"
#include "msg.h"
#include "wrapper_hash_set.h"

Graph::Graph (CvrStgFile *cvr, const BitString& emb, Selector& sel)
{
	Globs.TheGraph = this ;

	File = cvr ;
	EmbValueModulus = File->getEmbValueModulus() ;
	SamplesPerVertex = File->getSamplesPerVertex() ;

	// construct sposs and tvalues
	std::vector<SamplePos*> sposs ;
	std::vector<EmbValue> tvalues ;
	unsigned long n = emb.getNAryLength() ;
	for (unsigned long i = 0 ; i < n ; i++) {
		SamplePos *poss = new SamplePos[File->getSamplesPerVertex()] ;

		EmbValue modulosum = 0 ;
		for (unsigned int j = 0 ; j < File->getSamplesPerVertex() ; j++) {
			poss[j] = sel[(i * File->getSamplesPerVertex()) + j] ;
			modulosum = (modulosum + File->getEmbeddedValue(poss[j])) % File->getEmbValueModulus() ;
		}

		if (modulosum != emb.getNAry(i)) {
			sposs.push_back (poss) ;
			tvalues.push_back (emb.getNAry(i)) ;
		}
		else {
			delete[] poss ;
		}
	}

	myassert (sposs.size() == tvalues.size()) ;

	std::vector<SampleValue**> svalues ; // will contain pointers to unique sample value objects
	constructSamples (sposs, svalues) ;

	myassert (sposs.size() == svalues.size()) ;

	constructVertices (sposs, svalues, tvalues) ;

	constructEdges () ;
}

void Graph::constructSamples (const std::vector<SamplePos*>& sposs, std::vector<SampleValue**>& svalues)
{
	const VertexLabel numvertices = sposs.size() ;
	svalues.resize (numvertices) ;

	// fill a hash table with the (unique) sample values and svalues with pointers to them
	sgi::hash_set<SampleValue*,SampleValueHash,SampleValuesEqual> SampleValues_set ;
	for (unsigned long i = 0 ; i < numvertices ; i++) {
		svalues[i] = new SampleValue*[File->getSamplesPerVertex()] ;
		for (unsigned short j = 0 ; j < File->getSamplesPerVertex() ; j++) {
			SampleValue *sv = File->getSampleValue (sposs[i][j]) ;
			sgi::hash_set<SampleValue*,SampleValueHash,SampleValuesEqual>::iterator res = SampleValues_set.find (sv) ;
			if (res == SampleValues_set.end()) { // sample has not been found - add it !
				SampleValues_set.insert (sv) ;
				svalues[i][j] = sv ;
			}
			else { // sample value is already in SampleValues_set
				delete sv ;
				svalues[i][j] = *res ;
			} 
		}
	}

	// move the hash_set SampleValues_set into the vector SampleValues, set sample labels
	SampleValues = std::vector<SampleValue*> (SampleValues_set.size()) ;
	unsigned long label = 0 ;
	for (sgi::hash_set<SampleValue*,SampleValueHash,SampleValuesEqual>::const_iterator i = SampleValues_set.begin() ; i != SampleValues_set.end() ; i++) {
		SampleValues[label] = *i ;
		SampleValues[label]->setLabel (label) ;
		label++ ;
	}
}

void Graph::constructVertices (std::vector<SamplePos*>& sposs, std::vector<SampleValue**>& svalues, const std::vector<EmbValue>& tvalues)
{
	const VertexLabel numvertices = sposs.size() ;
	Vertices = std::vector<Vertex*> (numvertices) ;
	for (VertexLabel i = 0 ; i < numvertices ; i++) {
		Vertices[i] = new Vertex (i, sposs[i], svalues[i], tvalues[i]) ;
	}
}

void Graph::constructEdges ()
{
	// create SampleValue Adjacency Lists
	SVALists = File->calcSVAdjacencyLists (SampleValues) ;

	// allocate memory for SampleOccurences
	SampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	DeletedSampleOccurences = std::vector<std::list<SampleOccurence> > (SampleValues.size()) ;
	NumSampleOccurences = std::vector<UWORD32*> (SampleValues.size()) ;
	NumDeletedSampleOccurences = std::vector<UWORD32*> (SampleValues.size()) ;

	for (SampleValueLabel lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		NumSampleOccurences[lbl] = new UWORD32[EmbValueModulus] ;
		NumDeletedSampleOccurences[lbl] = new UWORD32[EmbValueModulus] ;
		for (unsigned short t = 0 ; t < EmbValueModulus ; t++) {
			NumSampleOccurences[lbl][t] = 0 ;
			NumDeletedSampleOccurences[lbl][t] = 0 ;
		}
	}

	// fill SampleOccurences
	for (std::vector<Vertex*>::iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		for (unsigned short j = 0 ; j < Globs.TheCvrStgFile->getSamplesPerVertex() ; j++) {
			SampleOccurence occ (*vit, j) ;
			SampleValueLabel lbl = (*vit)->getSampleValue(j)->getLabel() ;
			std::list<SampleOccurence>::iterator occit = SampleOccurences[lbl].insert (SampleOccurences[lbl].end(), occ) ;
			NumSampleOccurences[lbl][(*vit)->getTargetValue(j)]++ ;
			(*vit)->setSampleOccurenceIt (j, occit) ;
		}
	}

	// compute NumEdges for all sample values
	for (SampleValueLabel srclbl = 0 ; srclbl < SampleValues.size() ; srclbl++) {
		for (EmbValue t = 0 ; t < EmbValueModulus ; t++) {
			UWORD32 numedges = 0 ;
			for (std::vector<SampleValue*>::const_iterator destsv = (*(SVALists[t]))[srclbl].begin() ;
					destsv != (*(SVALists[t]))[srclbl].end() ; destsv++) {
				numedges += NumSampleOccurences[(*destsv)->getLabel()][SampleValues[srclbl]->getEmbeddedValue()] ;
			}
			SampleValues[srclbl]->setNumEdges(t, numedges) ;
		}
	}
}

Graph::~Graph()
{
	for (std::vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		delete *i ;
	}

	for (std::vector<SampleValue*>::iterator svit = SampleValues.begin() ; svit != SampleValues.end() ; svit++) {
		delete *svit ;
	}

	for (EmbValue t = 0 ; t < EmbValueModulus ; t++) {
		delete SVALists[t] ;
	}

	for (SampleValueLabel lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		delete[] NumSampleOccurences[lbl] ;
		delete[] NumDeletedSampleOccurences[lbl] ;
	}
}

void Graph::unmarkDeletedAllVertices ()
{
	for (std::vector<Vertex*>::iterator it = Vertices.begin() ; it != Vertices.end() ; it++) {
		(*it)->unmarkDeleted() ;
	}
}

std::list<SampleOccurence>::iterator Graph::markDeletedSampleOccurence (std::list<SampleOccurence>::iterator it)
{
	Vertex *v = it->getVertex() ;
	unsigned short i = it->getIndex() ;
	SampleValueLabel lbl = v->getSampleValue(i)->getLabel() ;
	SampleOccurences[lbl].erase (it) ;
	NumSampleOccurences[lbl][v->getTargetValue(i)]-- ;
	NumDeletedSampleOccurences[lbl][v->getTargetValue(i)]++ ;
	return DeletedSampleOccurences[lbl].insert (DeletedSampleOccurences[lbl].end(), SampleOccurence (v, i)) ;
}

std::list<SampleOccurence>::iterator Graph::unmarkDeletedSampleOccurence (std::list<SampleOccurence>::iterator it)
{
	Vertex *v = it->getVertex() ;
	unsigned short i = it->getIndex() ;
	SampleValueLabel lbl = v->getSampleValue(i)->getLabel() ;
	DeletedSampleOccurences[lbl].erase (it) ;
	NumDeletedSampleOccurences[lbl][v->getTargetValue(i)]-- ;
	NumSampleOccurences[lbl][v->getTargetValue(i)]++ ;
	return SampleOccurences[lbl].insert (SampleOccurences[lbl].end(), SampleOccurence (v, i)) ;
}

float Graph::getAvgVertexDegree () const
{
	unsigned long sumdeg = 0 ;
	for (std::vector<Vertex*>::const_iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		sumdeg += (*vit)->getDegree() ;
	}
	return ((float) sumdeg / (float) Vertices.size()) ;
}

void Graph::printVerboseInfo()
{
	if (Args.Verbosity.getValue() == VERBOSE || Args.Verbosity.getValue() == STATS) {
		unsigned long sumdeg = 0 ;
		unsigned long mindeg = ULONG_MAX ;
		unsigned long maxdeg = 0 ;
		for (std::vector<Vertex*>::iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
			unsigned long deg = (*i)->getDegree() ;

			sumdeg += deg ;
			if (deg < mindeg) {
				mindeg = deg ;
			}
			if (deg > maxdeg) {
				maxdeg = deg ;
			}
		}
		float avgdeg = ((float) sumdeg / (float) Vertices.size()) ;
		myassert (sumdeg % 2 == 0) ;

		if (Args.Verbosity.getValue() == STATS) {
			printf ("%lu:%lu:%lu:%lu:%lu:%.1f:",
					(unsigned long) SampleValues.size(),	// number of distinct sample values
					(unsigned long) Vertices.size(),	// number of vertices
					sumdeg / 2, // number of edges
					mindeg, // minimum vertex degree
					maxdeg, // maximum vertex degree
					avgdeg // average vertex degree
				   ) ;
		}
		else { // Verbosity is VERBOSE
			VerboseMessage vmsg1 (_(" %lu sample values, %lu vertices, %lu edges"), SampleValues.size(), Vertices.size(), sumdeg / 2) ;
			vmsg1.printMessage() ;
		}
	}
}

bool Graph::check (bool verbose) const
{
	bool retval = true ;
	retval = check_Vertices(verbose) && retval ;
	retval = check_SampleValues(verbose) && retval ;
	retval = check_SampleOccurences(verbose) && retval ;
	retval = check_SVALists(verbose) && retval ;
	return retval ;
}

bool Graph::check_Vertices (bool verbose) const
{
	bool label_consistency = true ;
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		label_consistency = (Vertices[i]->getLabel() == i) && label_consistency ;
	}

	// check if SampleValue pointers are the same as in SampleValues data structure
	bool svuniqueness = true ;
	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		for (unsigned short j = 0 ; j < File->getSamplesPerVertex() ; j++) {
			SampleValue* sv = Vertices[i]->getSampleValue(j) ;
			svuniqueness = (sv == SampleValues[sv->getLabel()]) && svuniqueness ;
		}
	}

	return label_consistency && svuniqueness ;
}

bool Graph::check_SampleValues (bool verbose) const
{
	unsigned long n = SampleValues.size() ;

	bool label_consistency = true ;
	for (unsigned long i = 0 ; i < n ; i++) {
		if (SampleValues[i]->getLabel() != i) {
			label_consistency = false ;
			if (verbose) {
				std::cerr << "----- FAILED: check_SampleValues -----" << std::endl ;
				std::cerr << "SamplesValue[" << i << "]->getLabel(): " << SampleValues[i]->getLabel() << std::endl ;
				std::cerr << "--------------------------------------" << std::endl ;
			}
		}
	}

	bool sv_uniqueness = true ;
	for (unsigned long i = 0 ; i < n ; i++) {
		for (unsigned long j = 0 ; j < n ; j++) {
			if (i != j) {
				if (*(SampleValues[i]) == *(SampleValues[j])) {
					sv_uniqueness = false ;
					if (verbose) {
						std::cerr << "----- FAILED: check_SampleValues -----" << std::endl ;
						std::cerr << "uniqueness violated with the following two samples:" << std::endl ;
						SampleValues[i]->print(1) ;
						SampleValues[j]->print(1) ;
						std::cerr << "--------------------------------------" << std::endl ;
					}	
				}
			}
		}
	}

	return (label_consistency && sv_uniqueness) ;
}

bool Graph::check_SampleOccurences (bool verbose) const
{
	bool retval = true ;
	retval = check_SampleOccurences_size (verbose) && retval ;
	retval = check_SampleOccurences_correctness (verbose) && retval ;
	retval = check_SampleOccurences_completeness (verbose) && retval ;
	return retval ;
}

bool Graph::check_SampleOccurences_size (bool verbose) const
{
	bool size = (SampleOccurences.size() == SampleValues.size()) ;
	if (!size && verbose) {
		std::cerr << std::endl << "---- FAILED: check_SampleOccurences_size ----" << std::endl ;
		std::cerr << "SampleOccurences.size(): " << SampleOccurences.size() << std::endl ;
		std::cerr << "SampleValues.size(): " << SampleValues.size() << std::endl ;
		std::cerr << "-------------------------------------" << std::endl ;
	}
	return size ;
}

bool Graph::check_SampleOccurences_correctness (bool verbose) const
{
	bool correctness = true ;

	for (unsigned long lbl = 0 ; lbl < SampleValues.size() ; lbl++) {
		for (std::list<SampleOccurence>::const_iterator socit = SampleOccurences[lbl].begin() ; socit != SampleOccurences[lbl].end() ; socit++) {
			Vertex *v = socit->getVertex() ;
			unsigned short idx = socit->getIndex() ;
			correctness = (v->getSampleValue(idx) == SampleValues[lbl]) && correctness ; // pointer equivalence
		}
	}

	return correctness ;
}

bool Graph::check_SampleOccurences_completeness (bool verbose) const
{
	bool completeness = true ;

	for (unsigned long vlbl = 0 ; vlbl < Vertices.size() ; vlbl++) {
		for (unsigned short j = 0 ; j < File->getSamplesPerVertex() ; j++) {
			SampleOccurence soc (Vertices[vlbl], j) ;
			SampleValueLabel svlbl = Vertices[vlbl]->getSampleValue(j)->getLabel() ;
			completeness = (find(SampleOccurences[svlbl].begin(), SampleOccurences[svlbl].end(), soc) != SampleOccurences[svlbl].end()) && completeness ;
		}
	}

	return completeness ;
}

bool Graph::check_SVALists (bool verbose) const
{
	bool retval = true ;
	retval = check_SVALists_size (verbose) && retval ;
	retval = check_SVALists_soundness (verbose) && retval ;
	retval = check_SVALists_sorted (verbose) && retval ;
	retval = check_SVALists_uniqueness (verbose) && retval ;
	retval = check_SVALists_completeness (verbose) && retval ;
	return retval ;
}

bool Graph::check_SVALists_size (bool verbose) const
{
	bool size = true ;
	for (EmbValue i = 0 ; i < File->getEmbValueModulus() ; i++) {
		size = (SVALists[i]->getNumRows() == SampleValues.size()) && size ;
		if ((SVALists[i]->getNumRows() != SampleValues.size()) && verbose) {
			std::cerr << std::endl << "---- FAILED: check_SVALists_size ----" << std::endl ;
			std::cerr << "SVALists[" << i << "]->getNumRows(): " << SVALists[i]->getNumRows() << std::endl ;
			std::cerr << "SampleValues.size(): " << SampleValues.size() << std::endl ;
			std::cerr << "-------------------------------------" << std::endl ;
		}
	}
	return size ;
}

bool Graph::check_SVALists_soundness (bool verbose) const
{
	unsigned long numsvs = SampleValues.size() ;

	// check if everything in SVALists[i][j] really is a neighbour of j and has the embedded value i
	bool target_ok = true ;
	bool neigh_ok = true ;
	for (EmbValue t = 0 ; t < File->getEmbValueModulus() ; t++) {
		for (SampleValueLabel srclbl = 0 ; srclbl < numsvs ; srclbl++) {
			SampleValue* srcsv = SampleValues[srclbl] ;
			const std::vector<SampleValue*> &row = (*(SVALists[t]))[srclbl];
			for (std::vector<SampleValue*>::const_iterator destsv = row.begin() ; destsv != row.end() ; destsv++) {
				if ((*destsv)->getEmbeddedValue() != t) {
					target_ok = false ;
					if (verbose) {
						std::cerr << std::endl << "---- FAILED: check_SVALists_soundness ----" << std::endl ;
						std::cerr << "in SVALists[" << (int) t << "][" << srclbl << "] is a sv with getEmbeddedValue() == " << (*destsv)->getEmbeddedValue() << std::endl ;
						std::cerr << "-------------------------------------" << std::endl ;
					}
				}
				if (!srcsv->isNeighbour(*destsv)) {
					neigh_ok = false ;
					if (verbose) {
						std::cerr << std::endl << "---- FAILED: check_SVALists_soundness ----" << std::endl ;
						std::cerr << "in SVALists[" << (int) t << "][" << srclbl << "] is a sv that is not a neighbour of " << srclbl << std::endl ;
						std::cerr << "-------------------------------------" << std::endl ;
					}
				}
			}
		}
	}

	return target_ok && neigh_ok ;
}

bool Graph::check_SVALists_sorted (bool verbose) const
{
	unsigned long numsvs = SampleValues.size() ;

	// check if SVALists[t][l][1...n] have increasing distance
	bool sorted = true ;
	for (EmbValue t = 0 ; t < File->getEmbValueModulus() ; t++) {
		for (SampleValueLabel srclbl = 0 ; srclbl < numsvs ; srclbl++) {
			SampleValue* srcsv = SampleValues[srclbl] ;
			const std::vector<SampleValue*> &row = (*(SVALists[t]))[srclbl] ;
			if (row.size() > 1) {
				for (unsigned int i = 0 ; i < (row.size() - 1) ; i++) {
					UWORD32 d1 = srcsv->calcDistance (row[i]) ;
					UWORD32 d2 = srcsv->calcDistance (row[i + 1]) ;
					if (!(d1 <= d2)) {
						sorted = false ;
						if (verbose) {
							std::cerr << std::endl << "---- FAILED: check_SVALists_sorted ----" << std::endl ;
							std::cerr << "source sample:" << std::endl ;
							srcsv->print(1) ;
							std::cerr << "dest sample at position " << i << ":" << std::endl ;
							row[i]->print(1) ;
							std::cerr << "dest sample at position " << i + 1 << ":" << std::endl ;
							row[i + 1]->print(1) ;
							std::cerr << "-------------------------------------" << std::endl ;
						}
					}
				}
			}
		}
	}

	return sorted ;
}

bool Graph::check_SVALists_uniqueness (bool verbose) const
{
	// check if there is no sample value that has two entries in an SVAList
	bool unique = true ;
	for (EmbValue t = 0 ; t < File->getEmbValueModulus() ; t++) {
		for (SampleValueLabel srclbl = 0 ; srclbl < SampleValues.size() ; srclbl++) {
			for (unsigned int i = 0 ; i < (*(SVALists[t]))[srclbl].size() ; i++) {
				for (unsigned int j = i + 1 ; j < (*(SVALists[t]))[srclbl].size() ; j++) {
					if (*((*(SVALists[t]))[srclbl][i]) == *((*(SVALists[t]))[srclbl][j])) {
						unique = false ;
						if (verbose) {
							std::cerr << std::endl << "---- FAILED: check_SVALists_uniqueness ----" << std::endl ;
							std::cerr << "SVALists[" << (int) t << "][" << srclbl << "][" << i << "]->getLabel() == " << (*(SVALists[t]))[srclbl][i]->getLabel() << std::endl ;
							std::cerr << "SVALists[" << (int) t << "][" << srclbl << "][" << j << "]->getLabel() == " << (*(SVALists[t]))[srclbl][j]->getLabel() << std::endl ;
							std::cerr << "-------------------------------------" << std::endl ;
						}
					}
				}
			}
		}
	}
	return unique ;
}


bool Graph::check_SVALists_completeness (bool verbose) const
{
	bool ok = true ;

	unsigned long numsvs = SampleValues.size() ;
	for (unsigned long i = 0 ; i < numsvs ; i++) {
		SampleValue *svsrc = SampleValues[i] ;
		for (unsigned long j = 0 ; j < numsvs ; j++) {
			SampleValue *svdest = SampleValues[j] ;
			if (svsrc->isNeighbour(svdest) && (svsrc->getLabel() != svdest->getLabel())) {
				// svsrc and svdest are neighbours => there must be an entry in SVALists
				myassert (svdest->isNeighbour (svsrc)) ;
				const std::vector<SampleValue*> &row = (*(SVALists[svdest->getEmbeddedValue()]))[i] ;
				bool found = false ;
				for (std::vector<SampleValue*>::const_iterator k = row.begin() ; k != row.end() ; k++) {
					if ((*k)->getLabel() == j) {
						found = true ;
					}
				}
				if (!found) {
					ok = false ;
					if (verbose) {
						std::cerr << std::endl << "---- FAILED: check_SVALists_completeness ----" << std::endl ;
						std::cerr << "sample values "<< svsrc->getLabel() << " and " << svdest->getLabel() << " are neighbours..." << std::endl ;
						std::cerr << "...but SVALists[" << (int) svdest->getEmbeddedValue() << "][" << i << "] does not contain " << j << std::endl ;
						std::cerr << "-------------------------------------" << std::endl ;
					}
				}
			}
		}
	}

	return ok ;
}

#ifdef DEBUG
void Graph::print (void) const
{
	unsigned long sumdeg = 0 ;
	for (std::vector<Vertex*>::const_iterator i = Vertices.begin() ; i != Vertices.end() ; i++) {
		sumdeg += (*i)->getDegree() ;
	}
	myassert (sumdeg % 2 == 0) ;
	std::cout << Vertices.size() << " " << (sumdeg / 2) << " U" << std::endl ;

	for (unsigned long i = 0 ; i < Vertices.size() ; i++) {
		std::cout << Vertices[i]->getDegree() << " " << (i + 1) << " 0 0" << std::endl ;

		for (unsigned short j = 0 ; j < File->getSamplesPerVertex() ; j++) {
			SampleValue *srcsample = Vertices[i]->getSampleValue(j) ;
			EmbValue srctarget = Vertices[i]->getTargetValue(j) ;
			for (unsigned long k = 0 ; k != Vertices.size() ; k++) {
				if (i != k) { // no loops
					for (unsigned short l = 0 ; l < File->getSamplesPerVertex() ; l++) {
						SampleValue *destsample = Vertices[k]->getSampleValue(l) ;
						EmbValue desttarget = Vertices[k]->getTargetValue(l) ;

						if ((srcsample->isNeighbour(destsample)) &&
							(srcsample->getEmbeddedValue() == desttarget) &&
							(destsample->getEmbeddedValue() == srctarget)) {
							std::cout << (k + 1) << " 0" << std::endl ;
						}
					}
				}
			}

		}
	}
}

void Graph::print_gml (std::ostream& out) const
{
	printPrologue_gml(out) ;
	srand ((unsigned int) time (NULL)) ;
	std::vector<bool> nodeprinted (Vertices.size()) ;
	std::vector<bool> edgesprinted (Vertices.size()) ;
	for (unsigned int i = 0 ; i < Vertices.size() ; i++) {
		printVertex_gml (out, Vertices[i], 1, nodeprinted, edgesprinted, false) ;
	}
	printEpilogue_gml(out) ;
}

void Graph::printVertex_gml (std::ostream& out, Vertex* vstart, unsigned int recdepth, std::vector<bool>& nodeprinted, std::vector<bool>& edgesprinted, bool start) const
{
	const float width = 1300.0 ;
	const float height = 1000.0 ;

	if (!nodeprinted[vstart->getLabel()]) {
		out << "    node [" << std::endl ;
		out << "        id " << vstart->getLabel() << std::endl ;
		std::string vlabel = "" ;
		for (unsigned short i = 0 ; i < File->getSamplesPerVertex() ; i++) {
			vlabel += vstart->getSampleValue(i)->getName() ;
			if (i != File->getSamplesPerVertex() - 1) {
				vlabel += "/" ;
			}
		}
		out << "        label \"" << vlabel << "\"" << std::endl ;
		out << "        graphics [" << std::endl ;
		out << "            x " << (width * (rand() / (RAND_MAX + 1.0))) << std::endl ;
		out << "            y " << (height * (rand() / (RAND_MAX + 1.0))) << std::endl ;
		out << "            w 10.0" << std::endl ;
		out << "            h 10.0" << std::endl ;
		out << "            type \"rectangle\"" << std::endl ;
		out << "            width 1.0" << std::endl ;
		if (start) {
			out << "            fill \"#00FF00\"" << std::endl ;
		}
		out << "        ]" << std::endl ;
		out << "    ]" << std::endl ;
	}
	nodeprinted[vstart->getLabel()] = true ;

	if (recdepth > 0) {
		EdgeIterator eit (vstart) ;
		while (!eit.isFinished()) {
			const Edge* e = *eit ;
			Vertex* vnext = e->getOtherVertex(vstart) ;

			if (!edgesprinted[vstart->getLabel()] && !edgesprinted[vnext->getLabel()]) {
				out << "    edge [" << std::endl ;
				out << "        source " << e->getVertex1()->getLabel() << std::endl ;
				out << "        target " << e->getVertex2()->getLabel() << std::endl ;
				out << "        label \"" ;
				out << e->getVertex1()->getSampleValue(e->getIndex1())->getName() << "/" ;
				out << e->getVertex2()->getSampleValue(e->getIndex2())->getName() << "\"" << std::endl ;
				out << "    ]" << std::endl ;
			}

			++eit ;
		}

		edgesprinted[vstart->getLabel()] = true ;

		eit.reset() ;
		while (!eit.isFinished()) {
			const Edge* e = *eit ;
			Vertex* vnext = e->getOtherVertex(vstart) ;

			printVertex_gml (out, vnext, recdepth - 1, nodeprinted, edgesprinted, false) ;

			++eit ;
		}
	}
}

void Graph::printPrologue_gml (std::ostream& out) const
{
	out << "graph [" << std::endl ;
	out << "    directed 0" << std::endl ;
}

void Graph::printEpilogue_gml (std::ostream& out) const
{
	out << "]" << std::endl ;
}

void Graph::print_Vertices (unsigned short spc) const
{
	char* space = new char[spc + 1] ;
	for (unsigned short i = 0 ; i < spc ; i++) {
		space[i] = ' ' ;
	}
	space[spc] = '\0' ;

	std::cerr << space << "Vertices:" << std::endl ;
	for (std::vector<Vertex*>::const_iterator vit = Vertices.begin() ; vit != Vertices.end() ; vit++) {
		(*vit)->print(spc + 1) ;
	}
}
#endif

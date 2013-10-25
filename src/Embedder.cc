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

#include <cfloat>
#include <cstdlib>

#include "DFSAPHeuristic.h"
#include "BitString.h"
#include "CvrStgFile.h"
#include "DMDConstructionHeuristic.h"
#include "EmbData.h"
#include "Embedder.h"
#include "Edge.h"
#include "Graph.h"
#include "Matching.h"
#include "ProgressOutput.h"
#include "Selector.h"
#include "Vertex.h"
#include "WKSConstructionHeuristic.h"
#include "common.h"
#include "error.h"
#include "msg.h"

Globals Globs ;

Embedder::Embedder ()
{
	// read embfile
	VerboseMessage vrs ;
	if (Args.EmbFn.getValue() == "") {
		vrs.setMessage (_("reading secret data from standard input...")) ;
	}
	else {
		vrs.setMessage (_("reading secret file \"%s\"..."), Args.EmbFn.getValue().c_str()) ;
		vrs.setNewline (false) ;
	}
	vrs.printMessage() ;

	std::vector<BYTE> emb ;
	BinaryIO embio (Args.EmbFn.getValue(), BinaryIO::READ) ;
	while (!embio.eof()) {
		emb.push_back (embio.read8()) ;
	}
	embio.close() ;

	VerboseMessage vdone (_(" done")) ;
	if (Args.EmbFn.getValue() != "") {
		vdone.printMessage() ;
	}

	// create bitstring to be embedded
	std::string fn = "" ;
	if (Args.EmbedEmbFn.getValue()) {
		fn = Args.EmbFn.getValue() ;
	}
	EmbData embdata (EmbData::EMBED, Args.Passphrase.getValue(), fn) ;
	embdata.setEncAlgo (Args.EncAlgo.getValue()) ;
	embdata.setEncMode (Args.EncMode.getValue()) ;
	embdata.setCompression (Args.Compression.getValue()) ;
	embdata.setChecksum (Args.Checksum.getValue()) ;
	embdata.setData (emb) ;
	ToEmbed = embdata.getBitString() ;

	// read cover-/stego-file
	VerboseMessage vrc ;
	if (Args.CvrFn.getValue() == "") {
		vrc.setMessage (_("reading cover file from standard input...")) ;
	}
	else {
		vrc.setMessage (_("reading cover file \"%s\"..."), Args.CvrFn.getValue().c_str()) ;
	}
	vrc.setNewline (false) ;
	vrc.printMessage() ;

	CvrStgFile::readFile (Args.CvrFn.getValue()) ;

	vdone.printMessage() ;

	ToEmbed.setArity (Globs.TheCvrStgFile->getEmbValueModulus()) ;
	if ((ToEmbed.getNAryLength() * Globs.TheCvrStgFile->getSamplesPerVertex()) > Globs.TheCvrStgFile->getNumSamples()) {
		throw SteghideError (_("the cover file is too short to embed the data.")) ;
	}

	// create graph
	Selector sel (Globs.TheCvrStgFile->getNumSamples(), Args.Passphrase.getValue()) ;

	VerboseMessage v (_("creating the graph...")) ;
	v.setNewline (false) ;
	v.printMessage() ;
	new Graph (Globs.TheCvrStgFile, ToEmbed, sel) ;
	Globs.TheGraph->printVerboseInfo() ;
	if (Args.Check.getValue()) {
		if (!Globs.TheGraph->check()) {
			CriticalWarning w ("integrity checking of graph data structures failed!") ; // TODO: internationalize this
			w.printMessage() ;
		}
	}

#ifdef DEBUG
	if (Args.DebugCommand.getValue() == PRINTGRAPH) {
		Globs.TheGraph->print() ;
		exit (EXIT_SUCCESS) ;
	}
	else if (Args.DebugCommand.getValue() == PRINTGMLGRAPH) {
		Globs.TheGraph->print_gml (std::cout) ;
		exit (EXIT_SUCCESS) ;
	}
	else if (Args.DebugCommand.getValue() == PRINTGMLVERTEX) {
		std::vector<bool> nodeprinted (Globs.TheGraph->getNumVertices()) ;
		std::vector<bool> edgesprinted (Globs.TheGraph->getNumVertices()) ;
		Globs.TheGraph->printPrologue_gml(std::cout) ;
		Globs.TheGraph->printVertex_gml (std::cout, Globs.TheGraph->getVertex(Args.GmlStartVertex.getValue()), Args.GmlGraphRecDepth.getValue(), nodeprinted, edgesprinted) ;
		Globs.TheGraph->printEpilogue_gml(std::cout) ;
		exit (EXIT_SUCCESS) ;
	}
#endif
}

Embedder::~Embedder ()
{
	delete Globs.TheGraph ;
	delete Globs.TheCvrStgFile ;
}
	
void Embedder::embed ()
{
	ProgressOutput* prout = NULL ;
	if (Args.Verbosity.getValue() == NORMAL) {
		std::string embstring, cvrstring ;
		if (Args.EmbFn.getValue() == "") {
			embstring = _("standard input") ;
		}
		else {
			embstring = "\"" + Args.EmbFn.getValue() + "\"" ;
		}
		if (Args.CvrFn.getValue() == "") {
			cvrstring = _("standard input") ;
		}
		else {
			cvrstring = "\"" + Args.CvrFn.getValue() + "\"" ;
		}
		char buf[200] ;
		sprintf (buf, _("embedding %s in %s..."), embstring.c_str(), cvrstring.c_str()) ;

		prout = new ProgressOutput (std::string(buf)) ;
	}
	else if (Args.Verbosity.getValue() == VERBOSE) {
		prout = new ProgressOutput () ;
	}

	const Matching* M = calculateMatching (prout) ;

	// embed matched edges
	const std::list<Edge*> medges = M->getEdges() ;
	for (std::list<Edge*>::const_iterator it = medges.begin() ; it != medges.end() ; it++) {
		embedEdge (*it) ;
	}

	// embed exposed vertices
	const std::list<Vertex*> *expvertices = M->getExposedVerticesLink() ;
	for (std::list<Vertex*>::const_iterator it = expvertices->begin() ; it != expvertices->end() ; it++) {
		embedExposedVertex (*it) ;
	}

	delete M ;

	// write stego file
	Globs.TheCvrStgFile->transform (Args.StgFn.getValue()) ;

	bool displaydone = false ;
	if (Globs.TheCvrStgFile->is_std()) {
		Message ws (_("writing stego file to standard output... ")) ;
		ws.printMessage() ;
	}
	else {
		if (Args.StgFn.getValue() != Args.CvrFn.getValue()) {
			Message ws (_("writing stego file \"%s\"... "), Globs.TheCvrStgFile->getName().c_str()) ;
			ws.setNewline (false) ;
			ws.printMessage() ;
			displaydone = true ;
		}
	}

	Globs.TheCvrStgFile->write() ;

	if (displaydone) {
		Message wsd (_("done")) ;
		wsd.printMessage() ;
	}
}

const Matching* Embedder::calculateMatching (ProgressOutput* prout)
{
	Matching* matching = new Matching (Globs.TheGraph, prout) ;

	std::vector<MatchingAlgorithm*> MatchingAlgos = Globs.TheCvrStgFile->getMatchingAlgorithms (Globs.TheGraph, matching) ;

	for (std::vector<MatchingAlgorithm*>::const_iterator ait = MatchingAlgos.begin() ; ait != MatchingAlgos.end() ; ait++) {
		if (Args.Verbosity.getValue() == VERBOSE) {
			prout->setMessage (_("executing %s..."), (*ait)->getName()) ;
		}

		(*ait)->setGoal (Args.Goal.getValue()) ;
		(*ait)->run() ;
		delete *ait ;

		if (Args.Verbosity.getValue() == VERBOSE) {
			prout->done (matching->getMatchedRate(), matching->getAvgEdgeWeight()) ;
		}
	}

	if (Args.Check.getValue()) {
		if (!matching->check()) {
			CriticalWarning w ("integrity checking of matching data structures failed!") ; // TODO: internationalize this
			w.printMessage() ;
		}
	}

	matching->printVerboseInfo() ; // only print info for best matching

	if (Args.Verbosity.getValue() == NORMAL) {
		prout->done() ;
	}

	if (prout) {
		delete prout ;
	}

	return matching ;
}
 
void Embedder::embedEdge (Edge *e)
{
	Vertex* v1 = e->getVertex1() ;
	Vertex* v2 = e->getVertex2() ;

	Globs.TheCvrStgFile->replaceSample (e->getSamplePos(v1), e->getReplacingSampleValue (v1)) ;
	Globs.TheCvrStgFile->replaceSample (e->getSamplePos(v2), e->getReplacingSampleValue (v2)) ;
}

void Embedder::embedExposedVertex (Vertex *v)
{
	SamplePos samplepos = 0 ;
	SampleValue *newsample = NULL ;
	float mindistance = FLT_MAX ;
	for (unsigned short i = 0 ; i < Globs.TheCvrStgFile->getSamplesPerVertex() ; i++) {
		SampleValue *curold = v->getSampleValue(i) ;
		SampleValue *curnew = v->getSampleValue(i)->getNearestTargetSampleValue(v->getTargetValue(i)) ;
		if (curold->calcDistance (curnew) < mindistance) {
			samplepos = v->getSamplePos(i) ;
			newsample = curnew ;
			mindistance = curold->calcDistance (curnew) ;
		}
		else {
			delete curnew ;
		}
	}

#ifdef DEBUG
	printDebug (1, "embedding vertex with label %lu by changing sample position %lu.", v->getLabel(), samplepos) ;
#endif

	EmbValue oldev = Globs.TheCvrStgFile->getEmbeddedValue (samplepos) ;
	Globs.TheCvrStgFile->replaceSample (samplepos, newsample) ;
	myassert (oldev != Globs.TheCvrStgFile->getEmbeddedValue (samplepos)) ;
	delete newsample ;
}

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
#include "CvrStgFile.h"
#define private public
#define protected public
#include "Graph.h"
#undef private
#undef protected
#include "Selector.h"

#include "utcommon.h"
#include "GraphTest.h"

GraphTest::GraphTest (TestSuite* s)
	: UnitTest ("Graph", s)
{
	ADDTESTCATEGORY (GraphTest, testSampleValues) ;
	ADDTESTCATEGORY (GraphTest, testVertices) ;
	ADDTESTCATEGORY (GraphTest, testSampleOccurences) ;
	ADDTESTCATEGORY (GraphTest, testSampleValueAdjacencyLists) ;
}

void GraphTest::setup ()
{
	UnitTest::setup() ;

	Globs.reset() ;
	f1 = CvrStgFile::readFile (std::string(DATADIR) + "win3x1_std.bmp") ;
	bs1 = new BitString (std::string ("this BitString will be embedded")) ;
	s1 = new Selector (bs1->getLength() * f1->getSamplesPerVertex(), std::string ("a passphrase")) ;
	g1 = new Graph (f1, *bs1, *s1) ;
	gl1 = Globs ;

	Globs.reset() ;
	f2 = CvrStgFile::readFile (std::string(DATADIR) + "win3x4_std.bmp") ;
	bs2 = new BitString (std::string ("this is a test")) ;
	s2 = new Selector (bs2->getLength() * f2->getSamplesPerVertex(), std::string ("another passphrase")) ;
	g2 = new Graph (f2, *bs2, *s2) ;
	gl2 = Globs ;

	Globs.reset() ;
	f3 = CvrStgFile::readFile (std::string(DATADIR) + "win3x8_std.bmp") ;
	bs3 = new BitString (std::string ("this is another test")) ;
	s3 = new Selector (bs3->getLength() * f3->getSamplesPerVertex(), std::string ("yet another passphrase")) ;
	g3 = new Graph (f3, *bs3, *s3) ;
	gl3 = Globs ;

	Globs.reset() ;
	f4 = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs4 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s4 = new Selector (bs4->getLength() * f4->getSamplesPerVertex(), std::string ("a true-color passphrase ;-)")) ;
	g4 = new Graph (f4, *bs4, *s4) ;
	gl4 = Globs ;

	Globs.reset() ;
	f5 = CvrStgFile::readFile (std::string(DATADIR) + "os21x1_std.bmp") ;
	bs5 = new BitString (std::string ("this BitString will be embedded")) ;
	s5 = new Selector (bs5->getLength() * f5->getSamplesPerVertex(), std::string ("a passphrase")) ;
	g5 = new Graph (f5, *bs5, *s5) ;
	gl5 = Globs ;

	Globs.reset() ;
	f6 = CvrStgFile::readFile (std::string(DATADIR) + "os21x4_std.bmp") ;
	bs6 = new BitString (std::string ("this is a test")) ;
	s6 = new Selector (bs6->getLength() * f6->getSamplesPerVertex(), std::string ("another passphrase")) ;
	g6 = new Graph (f6, *bs6, *s6) ;
	gl6 = Globs ;

	Globs.reset() ;
	f7 = CvrStgFile::readFile (std::string(DATADIR) + "os21x8_std.bmp") ;
	bs7 = new BitString (std::string ("this is another test")) ;
	s7 = new Selector (bs7->getLength() * f7->getSamplesPerVertex(), std::string ("yet another passphrase")) ;
	g7 = new Graph (f7, *bs7, *s7) ;
	gl7 = Globs ;

	Globs.reset() ;
	f8 = CvrStgFile::readFile (std::string(DATADIR) + "os21x24_std.bmp") ;
	bs8 = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s8 = new Selector (bs8->getLength() * f8->getSamplesPerVertex(), std::string ("a true-color passphrase ;-)")) ;
	g8 = new Graph (f8, *bs8, *s8) ;
	gl8 = Globs ;

	Globs.reset() ;
	f9 = CvrStgFile::readFile (std::string(DATADIR) + "mulaw_std.au") ;
	bs9 = new BitString (std::string ("u-law or mu-law that's the question")) ;
	s9 = new Selector (bs9->getLength() * f9->getSamplesPerVertex(), std::string ("muuuuuuuu")) ;
	g9 = new Graph (f9, *bs9, *s9) ;
	gl9 = Globs ;

	Globs.reset() ;
	f10 = CvrStgFile::readFile (std::string(DATADIR) + "pcm8_std.au") ;
	bs10 = new BitString (std::string ("also au files can have pcm encoded data")) ;
	s10 = new Selector (bs10->getLength() * f10->getSamplesPerVertex(), std::string ("pulse code modulation")) ;
	g10 = new Graph (f10, *bs10, *s10) ;
	gl10 = Globs ;

	Globs.reset() ;
	f11 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.au") ;
	bs11 = new BitString (std::string ("even with 16 bits per sample")) ;
	s11 = new Selector (bs11->getLength() * f11->getSamplesPerVertex(), std::string ("pcm16")) ;
	g11 = new Graph (f11, *bs11, *s11) ;
	gl11 = Globs ;

	Globs.reset() ;
	f12 = CvrStgFile::readFile (std::string(DATADIR) + "pcm8_std.wav") ;
	bs12 = new BitString (std::string ("8 bit pcm data in a riff wav file")) ;
	s12 = new Selector (bs12->getLength() * f12->getSamplesPerVertex(), std::string ("afaf")) ;
	g12 = new Graph (f12, *bs12, *s12) ;
	gl12 = Globs ;

	Globs.reset() ;
	f13 = CvrStgFile::readFile (std::string(DATADIR) + "pcm16_std.wav") ;
	bs13 = new BitString (std::string ("16 bits per sample")) ;
	s13 = new Selector (bs13->getLength() * f13->getSamplesPerVertex(), std::string ("passphrase")) ;
	g13 = new Graph (f13, *bs13, *s13) ;
	gl13 = Globs ;

	Globs.reset() ;
	f14 = CvrStgFile::readFile (std::string(DATADIR) + "std.jpg") ;
	bs14 = new BitString (std::string ("standard jpeg produced by the GIMP")) ;
	s14 = new Selector (bs14->getLength() * f14->getSamplesPerVertex(), std::string ("passphrase")) ;
	g14 = new Graph (f14, *bs14, *s14) ;
	gl14 = Globs ;

	Globs.reset() ;
	f15 = CvrStgFile::readFile (std::string(DATADIR) + "prog.jpg") ;
	bs15 = new BitString (std::string ("progressive encoding")) ;
	s15 = new Selector (bs15->getLength() * f15->getSamplesPerVertex(), std::string ("passphrase")) ;
	g15 = new Graph (f15, *bs15, *s15) ;
	gl15 = Globs ;

	Globs.reset() ;
	f_f = CvrStgFile::readFile (std::string(DATADIR) + "win3x24_std.bmp") ;
	bs_f = new BitString (std::string ("this time embedding in RGB pixel data")) ;
	s_f = new Selector (bs_f->getLength() * f_f->getSamplesPerVertex(), std::string ("a true-color passphrase ;-)")) ;
	g_f = new Graph (f_f, *bs_f, *s_f) ;
	gl_f = Globs ;
}

void GraphTest::cleanup ()
{
	UnitTest::cleanup() ;

	delete f1 ; delete bs1 ; delete s1 ; delete g1 ;
	delete f2 ; delete bs2 ; delete s2 ; delete g2 ;
	delete f3 ; delete bs3 ; delete s3 ; delete g3 ;
	delete f4 ; delete bs4 ; delete s4 ; delete g4 ;
	delete f5 ; delete bs5 ; delete s5 ; delete g5 ;
	delete f6 ; delete bs6 ; delete s6 ; delete g6 ;
	delete f7 ; delete bs7 ; delete s7 ; delete g7 ;
	delete f8 ; delete bs8 ; delete s8 ; delete g8 ;
	delete f9 ; delete bs9 ; delete s9 ; delete g9 ;
	delete f10 ; delete bs10 ; delete s10 ; delete g10 ;
	delete f11 ; delete bs11 ; delete s11 ; delete g11 ;
	delete f12 ; delete bs12 ; delete s12 ; delete g12 ;
	delete f13 ; delete bs13 ; delete s13 ; delete g13 ;
	delete f14 ; delete bs14 ; delete s14 ; delete g14 ;
	delete f15 ; delete bs15 ; delete s15 ; delete g15 ;
	// *_f are not delted because inconsistencies might crash the destructors
}

void GraphTest::testVertices()
{
	Globs = gl1 ; addTestResult (g1->check_Vertices(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_Vertices(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_Vertices(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_Vertices(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_Vertices(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_Vertices(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_Vertices(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_Vertices(ArgVerbose)) ;
	Globs = gl9 ; addTestResult (g9->check_Vertices(ArgVerbose)) ;
	Globs = gl10 ; addTestResult (g10->check_Vertices(ArgVerbose)) ;
	Globs = gl11 ; addTestResult (g11->check_Vertices(ArgVerbose)) ;
	Globs = gl12 ; addTestResult (g12->check_Vertices(ArgVerbose)) ;
	Globs = gl13 ; addTestResult (g13->check_Vertices(ArgVerbose)) ;
	Globs = gl14 ; addTestResult (g14->check_Vertices(ArgVerbose)) ;
	Globs = gl15 ; addTestResult (g15->check_Vertices(ArgVerbose)) ;

	// violate label consistency
	Globs = gl_f ;
	Vertex* tmp = g_f->Vertices[10] ;
	g_f->Vertices[10] = g_f->Vertices[11] ;
	g_f->Vertices[11] = tmp ;
	addTestResult (!g_f->check_Vertices()) ;
}

void GraphTest::testSampleValues()
{
	Globs = gl1 ; addTestResult (g1->check_SampleValues(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_SampleValues(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_SampleValues(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_SampleValues(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_SampleValues(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_SampleValues(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_SampleValues(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_SampleValues(ArgVerbose)) ;
	Globs = gl9 ; addTestResult (g9->check_SampleValues(ArgVerbose)) ;
	Globs = gl10 ; addTestResult (g10->check_SampleValues(ArgVerbose)) ;
	Globs = gl11 ; addTestResult (g11->check_SampleValues(ArgVerbose)) ;
	Globs = gl12 ; addTestResult (g12->check_SampleValues(ArgVerbose)) ;
	Globs = gl13 ; addTestResult (g13->check_SampleValues(ArgVerbose)) ;
	Globs = gl14 ; addTestResult (g14->check_SampleValues(ArgVerbose)) ;
	Globs = gl15 ; addTestResult (g15->check_SampleValues(ArgVerbose)) ;

	// violate uniqueness
	Globs = gl_f ;
	g_f->SampleValues[0] = g_f->SampleValues[1] ;
	addTestResult (!g_f->check_SampleValues()) ;
}

void GraphTest::testSampleOccurences()
{
	Globs = gl1 ; addTestResult (g1->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl9 ; addTestResult (g9->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl10 ; addTestResult (g10->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl11 ; addTestResult (g11->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl12 ; addTestResult (g12->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl13 ; addTestResult (g13->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl14 ; addTestResult (g14->check_SampleOccurences(ArgVerbose)) ;
	Globs = gl15 ; addTestResult (g15->check_SampleOccurences(ArgVerbose)) ;
}

void GraphTest::testSampleValueAdjacencyLists ()
{
	Globs = gl1 ; addTestResult (g1->check_SVALists(ArgVerbose)) ;
	Globs = gl2 ; addTestResult (g2->check_SVALists(ArgVerbose)) ;
	Globs = gl3 ; addTestResult (g3->check_SVALists(ArgVerbose)) ;
	Globs = gl4 ; addTestResult (g4->check_SVALists(ArgVerbose)) ;
	Globs = gl5 ; addTestResult (g5->check_SVALists(ArgVerbose)) ;
	Globs = gl6 ; addTestResult (g6->check_SVALists(ArgVerbose)) ;
	Globs = gl7 ; addTestResult (g7->check_SVALists(ArgVerbose)) ;
	Globs = gl8 ; addTestResult (g8->check_SVALists(ArgVerbose)) ;
	Globs = gl9 ; addTestResult (g9->check_SVALists(ArgVerbose)) ;
	Globs = gl10 ; addTestResult (g10->check_SVALists(ArgVerbose)) ;
	Globs = gl11 ; addTestResult (g11->check_SVALists(ArgVerbose)) ;
	Globs = gl12 ; addTestResult (g12->check_SVALists(ArgVerbose)) ;
	Globs = gl13 ; addTestResult (g13->check_SVALists(ArgVerbose)) ;
	Globs = gl14 ; addTestResult (g14->check_SVALists(ArgVerbose)) ;
	Globs = gl15 ; addTestResult (g15->check_SVALists(ArgVerbose)) ;
}

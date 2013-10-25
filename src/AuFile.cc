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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "AuData.h"
#include "AuFile.h"
#include "AuSampleValues.h"
#include "CvrStgFile.h"
#include "CvrStgObject.h"
#include "DFSAPHeuristic.h"
#include "SMDConstructionHeuristic.h"
#include "common.h"
#include "error.h"

AuFile::AuFile (BinaryIO *io)
	: CvrStgFile()
{
	setSamplesPerVertex (SamplesPerVertex) ;
	setEmbValueModulus (EmbValueModulus) ;
	read (io) ;
}

AuFile::~AuFile (void)
{
	delete Data ;
}

void AuFile::read (BinaryIO *io)
{
	CvrStgFile::read (io) ;

	try {
		Header.id[0] = '.' ;
		Header.id[1] = 's' ;
		Header.id[2] = 'n' ;
		Header.id[3] = 'd' ;

		// read header
		Header.offset = getBinIO()->read32_be() ;
		Header.size = getBinIO()->read32_be() ;
		UWORD32 encoding = getBinIO()->read32_be() ;
		Header.samplerate = getBinIO()->read32_be() ;
		Header.channels = getBinIO()->read32_be() ;

		// read infofield
		unsigned long leninfofield = Header.offset - AuHeader::HeaderSize ;
		if (leninfofield > 0) {
			Infofield.resize (leninfofield) ;
			for (unsigned long i = 0 ; i < leninfofield ; i++) {
				Infofield[i] = getBinIO()->read8() ;
			}
		}

		// read data
		switch (encoding) {
			case MULAW8:
			setRadius (Radius_MuLaw8) ;
			Data = new AuMuLawAudioData (this) ;
			break ;

			case PCM8:
			setRadius (Radius_PCM8) ;
			Data = new AuPCM8AudioData (this) ;
			break ;

			case PCM16:
			setRadius (Radius_PCM16) ;
			Data = new AuPCM16AudioData (this) ;
			break ;

			default:
			if (getBinIO()->is_std()) {
				throw NotImplementedError (_("the au file on standard input uses the unkown encoding %d."), encoding) ;
			}
			else {
				throw NotImplementedError (_("the au file \"%s\" uses the unknown encoding %d."), getBinIO()->getName().c_str(), encoding) ;
			}
			break ;
		} ;
		Header.encoding = (ENCODING) encoding ;
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while reading the au headers from the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::FILE_EOF:
			{
				throw SteghideError (_("premature end of file \"%s\" while reading au headers."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::STDIN_ERR:
			{
				throw SteghideError (_("an error occured while reading the au headers from standard input.")) ;
				break ;
			}

			case BinaryInputError::STDIN_EOF:
			{
				throw SteghideError (_("premature end of data from standard input while reading au headers.")) ;
				break ;
			}
		}
	}

	// if available, use size of audio data, else read until eof
	unsigned long n = AudioData::NoLimit ;
	if (Header.size != AuHeader::SizeUnknown) {
		myassert (Header.size % Header.getBytesPerSample() == 0) ;
		n = Header.size / Header.getBytesPerSample() ;
	}
	Data->read (getBinIO(), n) ;
}

void AuFile::write ()
{
	CvrStgFile::write() ;

	try {
		// write header
		getBinIO()->write8 (Header.id[0]) ;
		getBinIO()->write8 (Header.id[1]) ;
		getBinIO()->write8 (Header.id[2]) ;
		getBinIO()->write8 (Header.id[3]) ;
		getBinIO()->write32_be (Header.offset) ;
		getBinIO()->write32_be (Header.size) ;
		getBinIO()->write32_be ((UWORD32) Header.encoding) ;
		getBinIO()->write32_be (Header.samplerate) ;
		getBinIO()->write32_be (Header.channels) ;

		// write infofield
		for (unsigned long i = 0 ; i < Infofield.size() ; i++) {
			getBinIO()->write8 (Infofield[i]) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while writing the au headers to the file \"%s\"."), getBinIO()->getName().c_str()) ;
				break ;
			}

			case BinaryOutputError::STDOUT_ERR:
			{
				throw SteghideError (_("an error occured while writing the au headers to standard output.")) ;
				break ;
			}
		}
	}

	Data->write (getBinIO(), AudioData::NoLimit) ;
}

std::list<CvrStgFile::Property> AuFile::getProperties () const
{
	std::list<CvrStgFile::Property> retval ;

	// format
	std::string formatstring = "au audio" ;
	switch (Header.encoding) {
		case MULAW8:
			formatstring += _(", mu-law encoding") ;
		break ;

		case PCM8:
		case PCM16:
			formatstring += _(", PCM encoding") ;
		break ;
	}
	retval.push_back (CvrStgFile::Property (_("format"), formatstring)) ;

	return retval ;
}

unsigned short AuFile::AuHeader::getBytesPerSample () const
{
	unsigned short retval = 0 ;
	switch (encoding) {
		case MULAW8:
		retval = 1 ;
		break ;

		case PCM8:
		retval = 1 ;
		break ;

		case PCM16:
		retval = 2 ;
		break ;

		default:
		myassert(0) ;
		break ;
	}
	return retval ;
}

std::vector<MatchingAlgorithm*> AuFile::getMatchingAlgorithms (Graph* g, Matching* m) const
{
	std::vector<MatchingAlgorithm*> retval ;
	retval.push_back (new SMDConstructionHeuristic (g, m)) ;
	retval.push_back (new DFSAPHeuristic (g, m)) ;
	return retval ;
}

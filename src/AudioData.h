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

#ifndef SH_AUDIODATA_H
#define SH_AUDIODATA_H

#include <vector>

#include "AudioSampleValue.h"
class BinaryIO ;
#include "CvrStgObject.h"

/**
 * \class AudioData
 * \brief interface definition for AudioData objects.
 *
 * This class is necessary to provide one common base class for all types
 * of audio data, i.e. all different instances of AudioDataImpl.
 **/
class AudioData : public CvrStgObject {
	public:
	/// constant that can be used as parameter to read and write to indicate that there is no limit
	static const UWORD32 NoLimit = 0 ;

	virtual void read (BinaryIO* io, UWORD32 n = NoLimit) = 0 ;
	virtual void write (BinaryIO* io, UWORD32 n = NoLimit) = 0 ;
} ;

/**
 * \class AudioDataImpl
 * \brief implementation of the AudioData-Interface
 **/
template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType = AudioSampleValue<Type,ValueType> >
class AudioDataImpl : public AudioData {
	public:
	AudioDataImpl (CvrStgFile* f) : TheCvrStgFile(f) {} ;
	virtual ~AudioDataImpl (void) {} ;

	void read (BinaryIO* io, UWORD32 n = AudioData::NoLimit) ;
	void write (BinaryIO* io, UWORD32 n = AudioData::NoLimit) ;

	unsigned long getNumSamples (void) const ;
	SampleValue* getSampleValue (const SamplePos pos) const ;
	void replaceSample (const SamplePos pos, const SampleValue* s) ;

	private:
	std::vector<ValueType> Data ;
	CvrStgFile* TheCvrStgFile ;

	ValueType readValue (BinaryIO* io) const ;
	void writeValue (BinaryIO* io, ValueType v) const ;
} ;

#include "error.h"

template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType>
void AudioDataImpl<Type,ValueType,SampleValueType>::read (BinaryIO* io, UWORD32 n)
{
	try {
		if (n == NoLimit) {
			Data.clear() ;
			while (!io->eof()) {
				Data.push_back (readValue(io)) ;
			}
		}
		else {
			Data.resize (n) ;
			for (UWORD32 i = 0 ; i < n ; i++) {
				Data[i] = readValue(io) ;
			}
		}
	}
	catch (BinaryInputError e) {
		switch (e.getType()) {
			case BinaryInputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while reading the audio data from the file \"%s\"."), io->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::FILE_EOF:
			{
				throw SteghideError (_("premature end of file \"%s\" while reading audio data."), io->getName().c_str()) ;
				break ;
			}

			case BinaryInputError::STDIN_ERR:
			{
				throw SteghideError (_("an error occured while reading the audio data from standard input.")) ;
				break ;
			}

			case BinaryInputError::STDIN_EOF:
			{
				throw SteghideError (_("premature end of data from standard input while reading audio data.")) ;
				break ;
			}
		}
	}
}

template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType>
void AudioDataImpl<Type,ValueType,SampleValueType>::write (BinaryIO* io, UWORD32 n)
{
	try {
		if (n == NoLimit) {
			n = Data.size() ;
		}
		for (UWORD32 i = 0 ; i < n ; i++) {
			writeValue (io, Data[i]) ;
		}
	}
	catch (BinaryOutputError e) {
		switch (e.getType()) {
			case BinaryOutputError::FILE_ERR:
			{
				throw SteghideError (_("an error occured while writing the audio data to the file \"%s\"."), io->getName().c_str()) ;
				break ;
			}

			case BinaryOutputError::STDOUT_ERR:
			{
				throw SteghideError (_("an error occured while writing the audio data to standard output.")) ;
				break ;
			}
		}
	}
}

template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType>
unsigned long AudioDataImpl<Type,ValueType,SampleValueType>::getNumSamples (void) const
{
	return Data.size() ;
}

template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType>
SampleValue* AudioDataImpl<Type,ValueType,SampleValueType>::getSampleValue (const SamplePos pos) const
{
	myassert (pos < Data.size()) ;
	return ((SampleValue*) new SampleValueType (Data[pos])) ;
}

template<AUDIOSAMPLETYPE Type, class ValueType, class SampleValueType>
void AudioDataImpl<Type,ValueType,SampleValueType>::replaceSample (const SamplePos pos, const SampleValue* s)
{
	const SampleValueType* sample = dynamic_cast<const SampleValueType*> (s) ;
	myassert (sample) ;
	myassert (pos < Data.size()) ;
	Data[pos] = sample->getValue() ;
}

#endif // ndef SH_AUDIODATA_H

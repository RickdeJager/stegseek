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
#include <iostream>
#include <vector>

#include "EmbData.h"
#include "Embedder.h"
#include "Extractor.h"
#include "CvrStgFile.h"
#include "MCryptPP.h"
#include "Session.h"
#include "Utils.h"
#include "common.h"
#include "error.h"

void Session::run ()
{
	switch (Args.Command.getValue()) {
		case EMBED: {
			Embedder emb ;
			emb.embed() ;
		break ; }

		case EXTRACT: {
			Extractor ext (Args.StgFn.getValue(), Args.Passphrase.getValue()) ;
			EmbData* embdata = ext.extract() ;

			// write data
			std::string fn ;
			if (Args.ExtFn.is_set()) {
				if (Args.ExtFn.getValue() == "") {
					// write extracted data to stdout
					fn = "" ;
				}
				else {
					// file name given by extracting user overrides embedded file name
					fn = Args.ExtFn.getValue() ;
				}
			}
			else {
				// write extracted data to file with embedded file name
				myassert (Args.ExtFn.getValue() == "") ;
				fn = embdata->getFileName() ;
				if (fn.length() == 0) {
					throw SteghideError (_("please specify a file name for the extracted data (there is no name embedded in the stego file).")) ;
				}
			}

			VerboseMessage vwe ;
			bool printdone = true ;
			if (fn == "") {
				vwe.setMessage (_("writing extracted data to standard output...")) ;
				printdone = false ;
			}
			else {
				vwe.setMessage (_("writing extracted data to \"%s\"..."), fn.c_str()) ;
				vwe.setNewline (false) ;
			}
			vwe.printMessage() ;

			BinaryIO io (fn, BinaryIO::WRITE) ;
			std::vector<BYTE> data = embdata->getData() ;
			for (std::vector<BYTE>::iterator i = data.begin() ; i != data.end() ; i++) {
				io.write8 (*i) ;
			}
			io.close() ;

			if (printdone) {
				VerboseMessage vdone (_(" done")) ;
				vdone.printMessage() ;
			}

			if (Args.Verbosity.getValue() < VERBOSE) {
				if (fn != "") {
					Message m (_("wrote extracted data to \"%s\"."), fn.c_str()) ;
					m.printMessage() ;
				}
			}
		break ; }

		case INFO: {
			printInfo() ;
		break ; }

		case ENCINFO: {
			printEncInfo() ;
		break ; }

		case SHOWVERSION: {
			printVersion() ;
		break ; }

		case SHOWLICENSE: {
			printLicense() ;
		break ; }

		case SHOWHELP: {
			printHelp() ;
		break ; }

#ifdef DEBUG
		case PRINTFREQS: {
			printFrequencies() ;
		break ; }
#endif

		default: {
			myassert (0) ;
		break ; }
	}
}

void Session::printInfo ()
{
	// if file format not supported: exception will be catched in main
	CvrStgFile* file = CvrStgFile::readFile (Args.CvrFn.getValue()) ;

	if (Args.CvrFn.getValue() == "") {
		printf (_("data from standard input:\n")) ;
	}
	else {
		printf ("\"%s\":\n", stripDir(Args.CvrFn.getValue()).c_str()) ;
	}

	std::list<CvrStgFile::Property> props = file->getProperties() ;
	props.push_back (CvrStgFile::Property (_("capacity"), file->getHRCapacity())) ;
	for (std::list<CvrStgFile::Property>::const_iterator it = props.begin() ; it != props.end() ; it++) {
		printf ("  %s: %s\n", it->getKey().c_str(), it->getValue().c_str()) ;
	}

	bool printembinfo = Args.Passphrase.is_set() ;
	if (!printembinfo) {
		Question q (_("Try to get information about embedded data ?")) ;
		q.printMessage() ;
		printembinfo = q.getAnswer() ;
	}

	if (printembinfo) {
		try {
			std::string pp ;
			if (Args.Passphrase.is_set()) {
				pp = Args.Passphrase.getValue() ;
			}
			else {
				pp = Args.getPassphrase() ; // ask user for it
			}

			Extractor e (Args.CvrFn.getValue(), pp) ;
			EmbData* embdata = e.extract() ;

			if (embdata->getFileName() == "") {
				printf (_("  embedded data:\n")) ;
			}
			else {
				printf (_("  embedded file \"%s\":\n"), embdata->getFileName().c_str()) ;
			}

			printf (_("    size: %s\n"), Utils::formatHRSize(embdata->getData().size()).c_str()) ;
			std::string encstring ;
			if (embdata->getEncAlgo() == EncryptionAlgorithm(EncryptionAlgorithm::NONE)) {
				encstring += _("no") ;
			}
			else {
				encstring += embdata->getEncAlgo().getStringRep() + ", " + embdata->getEncMode().getStringRep() ;
			}
			printf (_("    encrypted: %s\n"), encstring.c_str()) ;
			printf (_("    compressed: %s\n"), ((embdata->getCompression() > 0) ? _("yes") : _("no"))) ;

			delete embdata ;
		}
		catch (CorruptDataError e) {
			printf (_("could not extract any data with that passphrase!\n")) ;
		}
	}
}

std::string Session::stripDir (std::string s) const
{
	unsigned int start = 0 ;
	if ((start = s.find_last_of ("/\\")) == std::string::npos) {
		start = 0 ;
	}
	else {
		start += 1 ;
	}
	return s.substr (start, std::string::npos) ;
}

void Session::printEncInfo ()
{
#ifdef USE_LIBMCRYPT
	std::vector<std::string> algos = MCryptPP::getListAlgorithms() ;
	std::vector<std::string> modes = MCryptPP::getListModes() ;

	printf (_("encryption algorithms:\n"
		"<algorithm>: <supported modes>...\n")) ;

	for (std::vector<std::string>::iterator a = algos.begin() ; a != algos.end() ; a++) {
		if (EncryptionAlgorithm::isValidStringRep (*a)) { // invalid if supported by libmcrypt but not by steghide, e.g. blowfish-compat
			std::cout << *a << ":" ;
			for (std::vector<std::string>::iterator m = modes.begin() ; m != modes.end() ; m++) {
				if (EncryptionMode::isValidStringRep (*m)) {
					if (MCryptPP::AlgoSupportsMode (*a, *m)) {
						std::cout << " " << *m ;
					}
				}
			}
			std::cout << std::endl ;
		}
	}
#else
	printf (_("steghide has been compiled without support for encryption.\n"
		"If you want to encrypt your data before embedding it, use an external encryption\n"
		"program or install libmcrypt (http://mcrypt.sourceforge.net/) and recompile steghide.\n")) ;
#endif
}

void Session::printVersion ()
{
	std::cout << "steghide version " << VERSION << std::endl ;
}

void Session::printHelp ()
{
	printVersion() ;
	printf (_("\n"
		"the first argument must be one of the following:\n"
		" embed, --embed          embed data\n"
		" extract, --extract      extract data\n"
		" info, --info            display information about a cover- or stego-file\n"
		"   info <filename>       display information about <filename>\n"
		" encinfo, --encinfo      display a list of supported encryption algorithms\n"
		" version, --version      display version information\n"
		" license, --license      display steghide's license\n"
		" help, --help            display this usage information\n"

		"\nembedding options:\n"
		" -ef, --embedfile        select file to be embedded\n"
		"   -ef <filename>        embed the file <filename>\n"
		" -cf, --coverfile        select cover-file\n"
		"   -cf <filename>        embed into the file <filename>\n"
		" -p, --passphrase        specify passphrase\n"
		"   -p <passphrase>       use <passphrase> to embed data\n"
		" -sf, --stegofile        select stego file\n"
		"   -sf <filename>        write result to <filename> instead of cover-file\n"
		" -e, --encryption        select encryption parameters\n"
		"   -e <a>[<m>]|<m>[<a>]  specify an encryption algorithm and/or mode\n"
		"   -e none               do not encrypt data before embedding\n"
		" -z, --compress          compress data before embedding (default)\n"
		"   -z <l>                 using level <l> (1 best speed...9 best compression)\n"
		" -Z, --dontcompress      do not compress data before embedding\n"
		" -K, --nochecksum        do not embed crc32 checksum of embedded data\n"
		" -N, --dontembedname     do not embed the name of the original file\n"
		" -f, --force             overwrite existing files\n"
		" -q, --quiet             suppress information messages\n"
		" -v, --verbose           display detailed information\n"

		"\nextracting options:\n"
		" -sf, --stegofile        select stego file\n"
		"   -sf <filename>        extract data from <filename>\n"
		" -p, --passphrase        specify passphrase\n"
		"   -p <passphrase>       use <passphrase> to extract data\n"
		" -xf, --extractfile      select file name for extracted data\n"
		"   -xf <filename>        write the extracted data to <filename>\n"
		" -f, --force             overwrite existing files\n"
		" -q, --quiet             suppress information messages\n"
		" -v, --verbose           display detailed information\n"

		"\noptions for the info command:\n"
		" -p, --passphrase        specify passphrase\n"
		"   -p <passphrase>       use <passphrase> to get info about embedded data\n"

		"\nTo embed emb.txt in cvr.jpg: steghide embed -cf cvr.jpg -ef emb.txt\n"
		"To extract embedded data from stg.jpg: steghide extract -sf stg.jpg\n")) ;
}

void Session::printLicense ()
{
 	printf (
		"Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>\n\n"

 		"This program is free software; you can redistribute it and/or\n"
 		"modify it under the terms of the GNU General Public License\n"
 		"as published by the Free Software Foundation; either version 2\n"
 		"of the License, or (at your option) any later version.\n\n"

 		"This program is distributed in the hope that it will be useful,\n"
 		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
 		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
 		"GNU General Public License for more details.\n\n"

 		"You should have received a copy of the GNU General Public License\n"
 		"along with this program; if not, write to the Free Software\n"
 		"Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n") ;
}

#ifdef DEBUG
void Session::printFrequencies ()
{
	std::map<SampleKey,unsigned long> Frequencies ;
	const std::list<std::string>& flist = Args.FileList.getValue() ;
	for (std::list<std::string>::const_iterator fnit = flist.begin() ; fnit != flist.end() ; fnit++) {
		CvrStgFile *f = CvrStgFile::readFile (*fnit) ;
		addFrequencies (&Frequencies, f->getFrequencies()) ;
		delete f ;
	}

	// get CvrStgFile in correct format to call printFrequencies
	CvrStgFile *f = CvrStgFile::readFile (*(Args.FileList.getValue().begin())) ;
	f->printFrequencies (Frequencies) ;
	delete f ;
}

void Session::addFrequencies (std::map<SampleKey,unsigned long>* f1, const std::map<SampleKey,unsigned long>* f2)
{
	for (std::map<SampleKey,unsigned long>::const_iterator fit = f2->begin() ; fit != f2->end() ; fit++) {
		(*f1)[fit->first] += fit->second ;
	}
}
#endif // def DEBUG

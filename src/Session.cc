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
 * 2020:
 *  - Modified to include StegSeek help and version
 *
 */

#include <cstdio>
#include <iostream>
#include <vector>

#include "CvrStgFile.h"
#include "EmbData.h"
#include "Embedder.h"
#include "Extractor.h"
#include "MCryptPP.h"
#include "PasswordCracker.h"
#include "SeedCracker.h"
#include "Session.h"
#include "Utils.h"
#include "common.h"
#include "error.h"

void Session::run() {
    switch (Args.Command.getValue()) {
    case EMBED: {
        Embedder emb;
        emb.embed();
        break;
    }

    case EXTRACT: {
        Extractor ext(Args.StgFn.getValue(), Args.Passphrase.getValue());
        EmbData *embdata = ext.extract();

        // write data
        std::string fn;
        if (Args.ExtFn.is_set()) {
            if (Args.ExtFn.getValue() == "") {
                // write extracted data to stdout
                fn = "";
            } else {
                // file name given by extracting user overrides embedded file name
                fn = Args.ExtFn.getValue();
            }
        } else {
            // write extracted data to file with embedded file name
            myassert(Args.ExtFn.getValue() == "");
            fn = embdata->getFileName();
            if (fn.length() == 0) {
                throw SteghideError(
                    _("please specify a file name for the extracted data (there is no "
                      "name embedded in the stego file)."));
            }
        }

        VerboseMessage vwe;
        bool printdone = true;
        if (fn == "") {
            vwe.setMessage(_("writing extracted data to standard output..."));
            printdone = false;
        } else {
            vwe.setMessage(_("writing extracted data to \"%s\"..."), fn.c_str());
            vwe.setNewline(false);
        }
        vwe.printMessage();

        BinaryIO io(fn, BinaryIO::WRITE);
        std::vector<BYTE> data = embdata->getData();
        for (std::vector<BYTE>::iterator i = data.begin(); i != data.end(); i++) {
            io.write8(*i);
        }
        io.close();

        if (printdone) {
            VerboseMessage vdone(_(" done"));
            vdone.printMessage();
        }

        if (Args.Verbosity.getValue() < VERBOSE) {
            if (fn != "") {
                Message m(_("wrote extracted data to \"%s\"."), fn.c_str());
                m.printMessage();
            }
        }
        break;
    }

    case CRACK: {
        printVersion();
        PasswordCracker pcr;
        pcr.crack();
        break;
    }

    case SEED_CRACK: {
        printVersion();
        SeedCracker scr;
        scr.crack();
        break;
    }

    case INFO: {
        printInfo();
        break;
    }

    case ENCINFO: {
        printEncInfo();
        break;
    }

    case SHOWVERSION: {
        printVersion();
        break;
    }

    case SHOWLICENSE: {
        printLicense();
        break;
    }

    case SHOWHELP: {
        printHelp();
        break;
    }

#ifdef DEBUG
    case PRINTFREQS: {
        printFrequencies();
        break;
    }
#endif

    default: {
        myassert(0);
        break;
    }
    }
}

void Session::printInfo() {
    // if file format not supported: exception will be caught in main
    CvrStgFile *file = CvrStgFile::readFile(Args.CvrFn.getValue());

    if (Args.CvrFn.getValue() == "") {
        printf(_("data from standard input:\n"));
    } else {
        printf("\"%s\":\n", Utils::stripDir(Args.CvrFn.getValue()).c_str());
    }

    std::list<CvrStgFile::Property> props = file->getProperties();
    props.push_back(CvrStgFile::Property(_("capacity"), file->getHRCapacity()));
    for (std::list<CvrStgFile::Property>::const_iterator it = props.begin(); it != props.end();
         it++) {
        printf("  %s: %s\n", it->getKey().c_str(), it->getValue().c_str());
    }

    bool printembinfo = Args.Passphrase.is_set();
    if (!printembinfo) {
        Question q(_("Try to get information about embedded data ?"));
        q.printMessage();
        printembinfo = q.getAnswer();
    }

    if (printembinfo) {
        try {
            std::string pp;
            if (Args.Passphrase.is_set()) {
                pp = Args.Passphrase.getValue();
            } else {
                pp = Args.getPassphrase(); // ask user for it
            }

            Extractor e(Args.CvrFn.getValue(), pp);
            EmbData *embdata = e.extract();

            if (embdata->getFileName() == "") {
                printf(_("  embedded data:\n"));
            } else {
                printf(_("  embedded file \"%s\":\n"), embdata->getFileName().c_str());
            }

            printf(_("    size: %s\n"), Utils::formatHRSize(embdata->getData().size()).c_str());
            std::string encstring;
            if (embdata->getEncAlgo() == EncryptionAlgorithm(EncryptionAlgorithm::NONE)) {
                encstring += _("no");
            } else {
                encstring += embdata->getEncAlgo().getStringRep() + ", " +
                             embdata->getEncMode().getStringRep();
            }
            printf(_("    encrypted: %s\n"), encstring.c_str());
            printf(_("    compressed: %s\n"),
                   ((embdata->getCompression() > 0) ? _("yes") : _("no")));

            delete embdata;
        } catch (const CorruptDataError &) {
            printf(_("could not extract any data with that passphrase!\n"));
        }
    }
}

void Session::printEncInfo() {
    std::vector<std::string> algos = MCryptPP::getListAlgorithms();
    std::vector<std::string> modes = MCryptPP::getListModes();

    printf(_("encryption algorithms:\n"
             "<algorithm>: <supported modes>...\n"));

    for (std::vector<std::string>::iterator a = algos.begin(); a != algos.end(); a++) {
        if (EncryptionAlgorithm::isValidStringRep(*a)) { // invalid if supported by libmcrypt but
                                                         // not by steghide, e.g. blowfish-compat
            std::cout << *a << ":";
            for (std::vector<std::string>::iterator m = modes.begin(); m != modes.end(); m++) {
                if (EncryptionMode::isValidStringRep(*m)) {
                    if (MCryptPP::AlgoSupportsMode(*a, *m)) {
                        std::cout << " " << *m;
                    }
                }
            }
            std::cout << std::endl;
        }
    }
}

void Session::printVersion() {
    fprintf(stderr, "StegSeek version %s\n", VERSION);
    if (Args.Verbosity.getValue() == VERBOSE) {
        printSteghideVersion();
    }
}

void Session::printHelp() {
    printVersion();
    printf(_("\n"
             "=== StegSeek Help ===\n"
             "To crack a stegofile:\n"
             "stegseek [stegofile.jpg] [wordlist.txt]\n"
             "\n"
             "Commands:\n"
             " --crack                 Crack a stego file using a wordlist. This "
             "is the default mode.\n"
             " --seed                  Crack a stego file by attempting all "
             "embedding patterns.\n"
             "                         This mode can be used to detect a file "
             "encoded by steghide.\n"
             "                         In case the file was encoded without "
             "encryption, this mode will\n"
             "                         even recover the embedded file.\n"
             "Positional arguments:\n"
             " --crack [stegofile.jpg] [wordlist.txt] [output.txt]\n"
             " --seed  [stegofile.jpg] [output.txt]\n"
             "\n"
             "Keyword arguments:\n"
             " -sf, --stegofile        select stego file\n"
             " -wl, --wordlist         select the wordlist file\n"
             " -xf, --extractfile      select file name for extracted data\n"
             " -t, --threads           set the number of threads. Defaults to "
             "the number of cores.\n"
             " -f, --force             overwrite existing files\n"
             " -v, --verbose           display detailed information\n"
             " -q, --quiet             hide performance metrics (can improve "
             "performance)\n"
             " -s, --skipdefault       don't add guesses to the wordlist (empty "
             "password, filename, ...)\n"
             "\n"));

    if (Args.Verbosity.getValue() == VERBOSE) {
        printf("=== Steghide Help ===\n");
        printSteghideHelp();
    } else {
        printf("Use \"stegseek --help -v\" to include steghide's help.\n");
    }
}

void Session::printSteghideVersion() {
    fprintf(stderr, "based on steghide version %s\n", STEGHIDE_VERSION);
}

void Session::printSteghideHelp() {
    printf(_("\n"
             "the first argument must be one of the following:\n"
             " --embed                 embed data\n"
             " --extract               extract data\n"
             " --info                  display information about a cover- or "
             "stego-file\n"
             " --encinfo               display a list of supported encryption "
             "algorithms\n"
             " --version               display version information\n"
             " --license               display StegSeek's license\n"
             " --help                  display this usage information\n"

             "\nembedding options:\n"
             " -ef, --embedfile        select file to be embedded\n"
             "   -ef <filename>        embed the file <filename>\n"
             " -cf, --coverfile        select cover-file\n"
             "   -cf <filename>        embed into the file <filename>\n"
             " -p, --passphrase        specify passphrase\n"
             "   -p <passphrase>       use <passphrase> to embed data\n"
             " -sf, --stegofile        select stego file\n"
             "   -sf <filename>        write result to <filename> instead of "
             "cover-file\n"
             " -e, --encryption        select encryption parameters\n"
             "   -e <a>[<m>]|<m>[<a>]  specify an encryption algorithm and/or mode\n"
             "   -e none               do not encrypt data before embedding\n"
             " -z, --compress          compress data before embedding (default)\n"
             "   -z <l>                 using level <l> (1 best speed...9 best "
             "compression)\n"
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
             "   -p <passphrase>       use <passphrase> to get info about embedded "
             "data\n"

             "\nTo embed emb.txt in cvr.jpg: stegseek --embed -cf cvr.jpg -ef "
             "emb.txt\n"
             "To extract embedded data from stg.jpg: stegseek --extract -sf "
             "stg.jpg\n"));
}

void Session::printLicense() {
    printf("Copyright (C) 2020 Rick de Jager ( https://github.com/rickdejager )\n\n"

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
           "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, "
           "USA.\n");
}

#ifdef DEBUG
void Session::printFrequencies() {
    std::map<SampleKey, unsigned long> Frequencies;
    const std::list<std::string> &flist = Args.FileList.getValue();
    for (std::list<std::string>::const_iterator fnit = flist.begin(); fnit != flist.end(); fnit++) {
        CvrStgFile *f = CvrStgFile::readFile(*fnit);
        addFrequencies(&Frequencies, f->getFrequencies());
        delete f;
    }

    // get CvrStgFile in correct format to call printFrequencies
    CvrStgFile *f = CvrStgFile::readFile(*(Args.FileList.getValue().begin()));
    f->printFrequencies(Frequencies);
    delete f;
}

void Session::addFrequencies(std::map<SampleKey, unsigned long> *f1,
                             const std::map<SampleKey, unsigned long> *f2) {
    for (std::map<SampleKey, unsigned long>::const_iterator fit = f2->begin(); fit != f2->end();
         fit++) {
        (*f1)[fit->first] += fit->second;
    }
}
#endif // def DEBUG

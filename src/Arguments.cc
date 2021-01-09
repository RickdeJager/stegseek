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
 *  - Modified to include stegseek arguments
 *  - Overhauled parser to allow for positional arguments
 */

#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include "MCryptPP.h"
#include "Terminal.h"
#include "common.h"
#include "error.h"
#include "msg.h"

// the global Arguments object
Arguments Args;

Arguments::Arguments(int argc, char *argv[]) {
    bool delete_next = false;
    for (int i = 1; i < argc; ++i) {
        TheArguments.push_back(std::string(argv[i]));

        if (delete_next) {
            // overwrite passphrase in argv in order to avoid that it can be read with
            // the ps command
            unsigned int len = strlen(argv[i]);
            for (unsigned int j = 0; j < len; j++) {
                argv[i][j] = ' ';
            }
            delete_next = false;
        }

        if (std::string(argv[i]) == "-p" || std::string(argv[i]) == "--passphrase") {
            delete_next = true;
        }
    }
}

void Arguments::parse() {
    // if there are no arguments -> show help
    if (TheArguments.empty()) {
        Command.setValue(SHOWHELP);
        return;
    }

    // Otherwise, we start by looking for a command.
    // A command must start with two dashes.

    ArgIt curarg = TheArguments.begin();

    parse_Command(curarg);
    setDefaults();

    // Next, parse all keyword arguments. This returns all unmatched arguments
    std::vector<std::string> leftOvers = parse_Arguments(curarg);
    // Parse the leftover arguments as positional arguments
    parse_Positional(leftOvers);

    // (command-specific) argument post-processing
    if (Command.getValue() == EMBED) {
        if ((CvrFn.getValue() == "") && (EmbFn.getValue() == "")) {
            throw ArgError(_("standard input cannot be used for cover data AND data "
                             "to be embedded."));
        }
        if (!StgFn.is_set() && CvrFn.is_set()) {
            StgFn.setValue(CvrFn.getValue());
            Force.setValue(true);
        }
    }

    if (Command.getValue() == EMBED || Command.getValue() == EXTRACT) {
        if (!Passphrase.is_set()) {
            // prompt for passphrase
            if (Command.getValue() == EMBED) {
                if ((CvrFn.getValue() == "") || (EmbFn.getValue() == "")) {
                    throw ArgError(_("if standard input is used, the passphrase must be "
                                     "specified on the command line."));
                }
                Passphrase.setValue(getPassphrase(true));
            } else if (Command.getValue() == EXTRACT) {
                if (StgFn.getValue() == "") {
                    throw ArgError(_("if standard input is used, the passphrase must be "
                                     "specified on the command line."));
                }
                Passphrase.setValue(getPassphrase());
            }
        }
    }
}

void Arguments::parse_Command(ArgIt &curarg) {
    std::string CommandString = *curarg;

    if (CommandString == "--crack") {
        Command.setValue(CRACK);
    } else if (CommandString == "--embed") {
        Command.setValue(EMBED);
    } else if (CommandString == "--extract") {
        Command.setValue(EXTRACT);
    } else if (CommandString == "--seed") {
        Command.setValue(SEED_CRACK);
    } else if (CommandString == "--info") {
        Command.setValue(INFO);
    } else if (CommandString == "--encinfo") {
        Command.setValue(ENCINFO);
    } else if (CommandString == "--version") {
        Command.setValue(SHOWVERSION);
    } else if (CommandString == "--license") {
        Command.setValue(SHOWLICENSE);
    } else if (CommandString == "--help") {
        Command.setValue(SHOWHELP);
    } else {
        // If none of these matched, assume cracking mode
        Command.setValue(CRACK);
        // Return without pushing curarg forwards (since we didn't
        // match anything)
        return;
    }
    curarg++;
}

void Arguments::parse_Positional(std::vector<std::string> positionalArgs) {
    // Decide what to do with the arguments, based on the command
    switch (Command.getValue()) {
    case CRACK: {
        ArgString *posPtr[]{&StgFn, &WordlistFn, &ExtFn};
        for (int i = 0; i < 3 && i < positionalArgs.size(); i++) {
            if (!posPtr[i]->is_set())
                posPtr[i]->setValue(positionalArgs[i]);
        }
    } break;
    case SEED_CRACK: {
        // Extract this file into that file
        ArgString *posPtr[]{&StgFn, &ExtFn};
        for (int i = 0; i < 2 && i < positionalArgs.size(); i++) {
            if (!posPtr[i]->is_set())
                posPtr[i]->setValue(positionalArgs[i]);
        }
    } break;
    case EMBED: {
        // Embed this in that and output there
        ArgString *posPtr[]{&EmbFn, &CvrFn, &StgFn};
        for (int i = 0; i < 3 && i < positionalArgs.size(); i++) {
            if (!posPtr[i]->is_set())
                posPtr[i]->setValue(positionalArgs[i]);
        }
    } break;
    case EXTRACT: {
        // Extract this file into that file
        ArgString *posPtr[]{&StgFn, &ExtFn};
        for (int i = 0; i < 2 && i < positionalArgs.size(); i++) {
            if (!posPtr[i]->is_set())
                posPtr[i]->setValue(positionalArgs[i]);
        }
    } break;
    case INFO:
        if (positionalArgs.size() > 0 && !CvrFn.is_set()) {
            CvrFn.setValue(positionalArgs[0]);
        }

    default:
        break;
    }
}

std::vector<std::string> Arguments::parse_Arguments(ArgIt &curarg) {
    // Anything that isn't a dashed argument (-v, -wl bla.txt, etc..)
    // is added to this vector
    std::vector<std::string> leftOvers;
    while (curarg != TheArguments.end()) {
        // Match any arguments that aren't of the shape -.*
        if (curarg->c_str()[0] != '-' || *curarg == "-") {
            // If the argument is a single '-', translate it to ""
            if (*curarg == "-") {
                leftOvers.push_back("");
            } else {
                leftOvers.push_back(*curarg);
            }
            curarg++;
            continue;
        }

        // Anything starting with "-" is a flag, and should be parsed.
        if (*curarg == "-ef" || *curarg == "--embedfile") {
            std::vector<COMMAND> compatible{EMBED};
            parse_Generic_String(curarg, compatible, &EmbFn);
        } else if (*curarg == "-xf" || *curarg == "--extractfile") {
            std::vector<COMMAND> compatible{EXTRACT, CRACK, SEED_CRACK};
            parse_Generic_String(curarg, compatible, &ExtFn);
        } else if (*curarg == "-cf" || *curarg == "--coverfile") {
            std::vector<COMMAND> compatible{EMBED};
            parse_Generic_String(curarg, compatible, &CvrFn);
        } else if (*curarg == "-sf" || *curarg == "--stegofile") {
            std::vector<COMMAND> compatible{EMBED, EXTRACT, CRACK, SEED_CRACK};
            parse_Generic_String(curarg, compatible, &StgFn);
        } else if (*curarg == "-wl" || *curarg == "--wordlist") {
            std::vector<COMMAND> compatible{CRACK};
            parse_Generic_String(curarg, compatible, &WordlistFn);
        } else if (*curarg == "-p" || *curarg == "--passphrase") {
            std::vector<COMMAND> compatible{};
            parse_Generic_String(curarg, compatible, &Passphrase);
        } else if (*curarg == "-K" || *curarg == "--nochecksum") {
            std::vector<COMMAND> compatible{};
            parse_Generic_Bool(curarg, compatible, &Checksum, false);
        } else if (*curarg == "-N" || *curarg == "--dontembedname") {
            std::vector<COMMAND> compatible{EMBED};
            parse_Generic_Bool(curarg, compatible, &EmbedEmbFn, false);
        } else if (*curarg == "-f" || *curarg == "--force") {
            std::vector<COMMAND> compatible{EMBED, EXTRACT, CRACK, SEED_CRACK};
            parse_Generic_Bool(curarg, compatible, &Force, true);
        } else if (*curarg == "-s" || *curarg == "--skipdefault") {
            std::vector<COMMAND> compatible{CRACK};
            parse_Generic_Bool(curarg, compatible, &SkipDefaultGuesses, true);
        }
        // If there is no generic parser available, use a specific parser
        else {
            if (parse_Compression(curarg))
                continue;
            if (parse_Encryption(curarg))
                continue;
            if (parse_Radius(curarg))
                continue;
            if (parse_Goal(curarg))
                continue;
            if (parse_Threading(curarg))
                continue;
            if (parse_Verbosity(curarg))
                continue;
            if (parse_Debug(curarg))
                continue; // TODO - rename Debug -> Undocumented

            throw ArgError(_("unknown argument \"%s\"."), curarg->c_str());
        }
    }
    return leftOvers;
}

bool Arguments::parse_Generic_String(ArgIt &curarg, std::vector<COMMAND> compatibleCommands,
                                     ArgString *destArg) {
    std::string argflag = *curarg;

    // A generic string argument requires two values, -someflag, somevalue
    if (++curarg == TheArguments.end()) {
        throw ArgError(_("the argument \"%s\" requires a value"), argflag.c_str());
    }
    bool compatible = false;
    // If the compatibleCommands is empty, this argument doesn't have conflicts.
    if (compatibleCommands.empty()) {
        compatible = true;
    }
    // Otherwise, we check whether the current command is specified as compatible
    else {
        for (COMMAND c : compatibleCommands) {
            if (Command.getValue() == c) {
                compatible = true;
            }
        }
    }
    if (!compatible) {
        throw ArgError(_("the argument \"%s\" is not compatible with the current command"),
                       argflag.c_str());
    }
    if (destArg->is_set()) {
        throw ArgError(_("the \"%s\" argument can be used only once."), argflag.c_str());
    }

    // Translate - to the empty string, to indicate stdin/stdout
    if (*curarg == "-") {
        destArg->setValue("");
    } else {
        destArg->setValue(curarg->c_str());
    }
    curarg++;
    return true;
}

bool Arguments::parse_Generic_Bool(ArgIt &curarg, std::vector<COMMAND> compatibleCommands,
                                   ArgBool *destArg, bool ifMatch) {
    bool compatible = false;
    // If the compatibleCommands is empty, this argument doesn't have conflicts.
    if (compatibleCommands.empty()) {
        compatible = true;
    }
    // Otherwise, we check whether the current command is specified as compatible
    else {
        for (COMMAND c : compatibleCommands) {
            if (Command.getValue() == c) {
                compatible = true;
            }
        }
    }
    if (!compatible) {
        throw ArgError(_("the argument \"%s\" is not compatible with the current command"),
                       curarg->c_str());
    }
    if (destArg->is_set()) {
        throw ArgError(_("the \"%s\" argument can be used only once."), curarg->c_str());
    } else {
        destArg->setValue(ifMatch);
    }
    curarg++;
    return true;
}

bool Arguments::parse_Threading(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-t" || *curarg == "--threads") {
        if (Command.getValue() != CRACK && Command.getValue() != SEED_CRACK) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\" "
                             "and \"%s\" commands."),
                           curarg->c_str(), "crack", "seed");
        }

        if (Threads.is_set()) {
            throw ArgError(_("the \"%s\" argument can be used only once."), curarg->c_str());
        }
        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by the number of threads."),
                           (curarg - 1)->c_str());
        } else {
            int tmp;
            if (sscanf(curarg->c_str(), "%d", &tmp) == 1) {
                if (tmp > 0) {
                    Threads.setValue(tmp);
                    found = true;
                    curarg++;
                } else {
                    throw ArgError(_("The number of threads must be greater than 0."));
                }
            } else {
                throw ArgError(_("The number of threads must be an integer"));
            }
        }
    }
    return found;
}

bool Arguments::parse_Compression(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-z" || *curarg == "--compress") {
        if (Command.getValue() != EMBED) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"embed\" command."),
                           curarg->c_str());
        }

        if (Compression.is_set()) {
            throw ArgError(_("the compression argument can be used only once."));
        }

        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by the compression level."),
                           (curarg - 1)->c_str());
        }

        int tmp = 0;
        sscanf(curarg->c_str(), "%d", &tmp);
        if (tmp < 1 || tmp > 9) {
            throw ArgError(_("\"%s\" is not a valid compression level."), curarg->c_str());
        }
        Compression.setValue(tmp);

        found = true;
        curarg++;
    } else if (*curarg == "-Z" || *curarg == "--dontcompress") {
        if (Command.getValue() != EMBED) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\" command."),
                           curarg->c_str(), "embed");
        }

        if (Compression.is_set()) {
            throw ArgError(_("the compression argument can be used only once."));
        }

        Compression.setValue(NoCompression);

        found = true;
        curarg++;
    }

    return found;
}

bool Arguments::parse_Encryption(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-e" || *curarg == "--encryption") {
        if (Command.getValue() != EMBED) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\" command."),
                           curarg->c_str(), "embed");
        }

        if (EncAlgo.is_set() || EncMode.is_set()) {
            throw ArgError(_("the encryption argument can be used only once."));
        }

        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by encryption parameters."),
                           (curarg - 1)->c_str());
        }

        std::string s1, s2;
        if ((*curarg)[0] == '-') {
            throw ArgError(_("the \"%s\" argument must be followed by encryption parameters."),
                           (curarg - 1)->c_str());
        } else {
            s1 = *curarg;
            if (curarg + 1 == TheArguments.end()) {
                s2 = "";
            } else {
                if ((*(curarg + 1))[0] == '-') {
                    s2 = "";
                } else {
                    ++curarg; // set to second encryption specifier
                    s2 = *curarg;
                }
            }
        }

        if (s1 == "none" && s2 == "") {
            EncAlgo.setValue(s1);
        } else {
            bool s1_isalgo = false, s1_ismode = false;
            bool s2_isalgo = false, s2_ismode = false;

            if (s1 != "") {
                s1_isalgo = EncryptionAlgorithm::isValidStringRep(s1);
                s1_ismode = EncryptionMode::isValidStringRep(s1);

                myassert(!(s1_isalgo && s1_ismode));
                if (!(s1_isalgo || s1_ismode)) {
                    throw SteghideError(_("\"%s\" is neither an algorithm nor a mode "
                                          "supported by libmcrypt."),
                                        s1.c_str());
                }
            }
            if (s2 != "") {
                s2_isalgo = EncryptionAlgorithm::isValidStringRep(s2);
                s2_ismode = EncryptionMode::isValidStringRep(s2);

                myassert(!(s2_isalgo && s2_ismode));
                if (!(s2_isalgo || s2_ismode)) {
                    throw SteghideError(_("\"%s\" is neither an algorithm nor a mode "
                                          "supported by libmcrypt."),
                                        s2.c_str());
                }
            }

            if (s1_isalgo && s2_isalgo) {
                throw SteghideError(_("\"%s\" and \"%s\" are both libmcrypt "
                                      "algorithms. please specify only one."),
                                    s1.c_str(), s2.c_str());
            }
            if (s1_ismode && s2_ismode) {
                throw SteghideError(_("\"%s\" and \"%s\" are both libmcrypt modes. "
                                      "please specify only one."),
                                    s1.c_str(), s2.c_str());
            }

            if (s1_isalgo) {
                EncAlgo.setValue(s1);
            }
            if (s1_ismode) {
                EncMode.setValue(s1);
            }
            if (s2_isalgo) {
                EncAlgo.setValue(s2);
            }
            if (s2_ismode) {
                EncMode.setValue(s2);
            }

            if (!MCryptPP::AlgoSupportsMode(EncAlgo.getValue(), EncMode.getValue())) {
                throw SteghideError(_("the encryption algorithm \"%s\" can not be used "
                                      "with the mode \"%s\"."),
                                    EncAlgo.getValue().getStringRep().c_str(),
                                    EncMode.getValue().getStringRep().c_str());
            }
        }
        found = true;
        curarg++;
    }

    return found;
}

bool Arguments::parse_Radius(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-r" || *curarg == "--radius") {
        if (Command.getValue() != EMBED) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\" command."),
                           curarg->c_str(), "embed");
        }

        if (Radius.is_set()) {
            throw ArgError(_("the radius argument can be used only once."));
        }

        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by the "
                             "neighbourhood radius."),
                           (curarg - 1)->c_str());
        }

        unsigned long tmp = 0;
        sscanf(curarg->c_str(), "%lu", &tmp);
        Radius.setValue(tmp);

        found = true;
        curarg++;
    }

    return found;
}

bool Arguments::parse_Goal(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-g" || *curarg == "--goal") {
        if (Command.getValue() != EMBED) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\" command."),
                           curarg->c_str(), "embed");
        }

        if (Goal.is_set()) {
            throw ArgError(_("the goal argument can be used only once."));
        }

        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by a number "
                             "between 0 and 100."),
                           (curarg - 1)->c_str());
        }

        float tmp = 0;
        sscanf(curarg->c_str(), "%f", &tmp);
        if (tmp < 0 || tmp > 100) {
            throw ArgError(_("the \"%s\" argument must be followed by a number "
                             "between 0 and 100."),
                           (curarg - 1)->c_str());
        }
        Goal.setValue(tmp);

        found = true;
        curarg++;
    }

    return found;
}

bool Arguments::parse_Verbosity(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "-q" || *curarg == "--quiet") {
        found = true;

        if (Command.getValue() != EMBED && Command.getValue() != EXTRACT &&
            Command.getValue() != CRACK && Command.getValue() != SEED_CRACK) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\", "
                             "\"%s\", \"%s\", and \"%s\" commands."),
                           curarg->c_str(), "embed", "extract", "crack", "seed");
        }

        if (Verbosity.is_set()) {
            throw ArgError(_("the \"%s\" argument cannot be used here because the "
                             "verbosity has already been set."),
                           curarg->c_str());
        }

        Verbosity.setValue(QUIET);
        curarg++;
    } else if (*curarg == "-v" || *curarg == "--verbose") {
        found = true;

        if (Command.getValue() != EMBED && Command.getValue() != EXTRACT &&
            Command.getValue() != CRACK && Command.getValue() != SEED_CRACK &&
            Command.getValue() != SHOWHELP && Command.getValue() != SHOWVERSION) {
            throw ArgError(_("the argument \"%s\" can only be used with the \"%s\", "
                             "\"%s\", \"%s\", \"%s\", \"%s\" and \"%s\" commands."),
                           curarg->c_str(), "embed", "extract", "crack", "seed", "version", "help");
        }

        if (Verbosity.is_set()) {
            throw ArgError(_("the \"%s\" argument cannot be used here because the "
                             "verbosity has already been set."),
                           curarg->c_str());
        }

        Verbosity.setValue(VERBOSE);
        curarg++;
    }

    return found;
}

bool Arguments::parse_Debug(ArgIt &curarg) {
    bool found = false;

    if (*curarg == "--printgraph") {
        if (DebugCommand.is_set()) {
            throw SteghideError(_("you cannot use more than one debug command at a time."));
        }

        DebugCommand.setValue(PRINTGRAPH);

        found = true;
        curarg++;
    } else if (*curarg == "--printgmlgraph") {
        if (DebugCommand.is_set()) {
            throw SteghideError(_("you cannot use more than one debug command at a time."));
        }

        DebugCommand.setValue(PRINTGMLGRAPH);

        found = true;
        curarg++;
    } else if (*curarg == "--printgmlvertex") {
        if (DebugCommand.is_set()) {
            throw SteghideError(_("you cannot use more than one debug command at a time."));
        }

        DebugCommand.setValue(PRINTGMLVERTEX);
        curarg++;

        int tmp = 0;
        sscanf(curarg->c_str(), "%d", &tmp);
        GmlGraphRecDepth.setValue(tmp);
        curarg++;
        sscanf(curarg->c_str(), "%d", &tmp);
        GmlStartVertex.setValue(tmp);

        found = true;
        curarg++;
    } else if (*curarg == "--printstats") {
        if (DebugCommand.is_set()) {
            throw SteghideError(_("you cannot use more than one debug command at a time."));
        }

        if (Verbosity.is_set()) {
            throw ArgError(_("the \"%s\" argument cannot be used here because the "
                             "verbosity has already been set."),
                           curarg->c_str());
        }

        Verbosity.setValue(STATS);

        found = true;
        ++curarg;
    } else if (*curarg == "--debuglevel") {
        if (DebugLevel.is_set()) {
            throw ArgError(_("the debug level argument can be used only once."));
        }

        if (++curarg == TheArguments.end()) {
            throw ArgError(_("the \"%s\" argument must be followed by the debug level."),
                           (curarg - 1)->c_str());
        }

        unsigned int tmp = 0;
        sscanf(curarg->c_str(), "%u", &tmp);
        DebugLevel.setValue(tmp);

        found = true;
        ++curarg;
    } else if (*curarg == "--check") {
        // TODO usual error checking (omitted due to message freeze)
        Check.setValue(true);

        found = true;
        ++curarg;
    }

    return found;
}

std::string Arguments::getPassphrase(bool doublecheck) {
    int c = EOF;

#ifndef HAVE_TERMIOS_H
    Warning w(_("unknown terminal. the passphrase you type now will be visible."));
    w.printMessage();
#endif

    std::cerr << _("Enter passphrase: ");
    Terminal term;
    term.EchoOff();

    std::string s1 = "";
    while ((c = std::cin.get()) != '\n') {
        s1 += c;
    }

    term.reset();
    std::cerr << std::endl;

    if (doublecheck) {
        std::cerr << _("Re-Enter passphrase: ");
        term.EchoOff();

        std::string s2 = "";
        while ((c = std::cin.get()) != '\n') {
            s2 += c;
        }

        term.reset();
        std::cerr << std::endl;

        if (s1 != s2) {
            throw SteghideError(_("the passphrases do not match."));
        }
    }

    return s1;
}

bool Arguments::stdin_isused() const {
    bool retval = false;
    if (Command.getValue() == EMBED && (EmbFn.getValue() == "" || CvrFn.getValue() == "")) {
        retval = true;
    } else if (Command.getValue() == EXTRACT && StgFn.getValue() == "") {
        retval = true;
    } else if (Command.getValue() == INFO && CvrFn.getValue() == "") {
        retval = true;
    }
    return retval;
}

void Arguments::setDefaults(void) {
    myassert(Command.is_set());

    EmbFn.setValue("", false);
    CvrFn.setValue("", false);
    EncAlgo.setValue(Default_EncAlgo, false);
    EncMode.setValue(Default_EncMode, false);
    Checksum.setValue(Default_Checksum, false);
    Compression.setValue(Default_Compression, false);
    EmbedEmbFn.setValue(Default_EmbedEmbFn, false);
    ExtFn.setValue("", false);
    Passphrase.setValue("", false);
    StgFn.setValue("", false);
    WordlistFn.setValue("/usr/share/wordlists/rockyou.txt", false);
    Threads.setValue(std::thread::hardware_concurrency(), false);
    Force.setValue(Default_Force, false);
    SkipDefaultGuesses.setValue(Default_SkipDefaultGuesses, false);
    Verbosity.setValue(Default_Verbosity, false);
    Radius.setValue(Default_Radius, false);
    Goal.setValue(Default_Goal, false);
    Check.setValue(Default_Check, false);
    DebugCommand.setValue(Default_DebugCommand, false);
    DebugLevel.setValue(Default_DebugLevel, false);
    GmlGraphRecDepth.setValue(Default_GmlGraphRecDepth, false);
    GmlStartVertex.setValue(Default_GmlStartVertex, false);
}

const EncryptionAlgorithm Arguments::Default_EncAlgo =
    EncryptionAlgorithm(EncryptionAlgorithm::RIJNDAEL128);
const EncryptionMode Arguments::Default_EncMode = EncryptionMode(EncryptionMode::CBC);

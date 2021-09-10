/*
 * Stegseek 0.6 - a steghide cracker
 * Copyright (C) 2021 Rick de Jager
 *
 * Based on the work of Stefan Hetzl <shetzl@chello.at>
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

#include "PasswordCracker.h"

#include "CvrStgFile.h"
#include "EmbData.h"
#include "Extractor.h"
#include "Selector.h"
#include "Utils.h"
#include "common.h"
#include "error.h"

PasswordCracker::PasswordCracker() {
    VerboseMessage::print("Using wordlist file \"%s\".\n", Args.WordlistFn.getValue().c_str());
}

void PasswordCracker::crack() {

    unsigned int threads = Args.Threads.getValue();

    // First stat the word list to get the file length
    struct stat wordlistStats;
    if (stat(Args.WordlistFn.getValue().c_str(), &wordlistStats) != 0) {
        throw SteghideError("could not open the wordlist \"%s\".",
                            Args.WordlistFn.getValue().c_str());
    }

    // Initialize threads
    std::vector<std::thread> ThreadPool;
    bool metricsEnabled = Args.Verbosity.getValue() != QUIET;

    // Add a thread to keep track of metrics
    if (metricsEnabled) {
        ThreadPool.push_back(
            std::thread([this, wordlistStats] { metrics(wordlistStats.st_size); }));
    }

    // Add n worker threads
    // A part must have at least len 1, otherwise no words will be read when
    // threads > size
    unsigned long part = ((unsigned long)wordlistStats.st_size) / threads;
    if (part <= 0) {
        part = 1;
    }

    for (unsigned int i = 0; i < threads; i++) {
        ThreadPool.push_back(std::thread([this, i, part, metricsEnabled] {
            consume(i * part, (i + 1) * part, metricsEnabled);
        }));
    }

    // Join all worker threads
    for (unsigned int i = 0; i < threads; i++) {
        ThreadPool.back().join();
        ThreadPool.pop_back();
    }

    // Set stopped flag and terminate the metrics thread
    stopped = true;
    if (metricsEnabled) {
        ThreadPool.back().join();
        ThreadPool.pop_back();
    }

    if (!success) {
        throw SteghideError("Could not find a valid passphrase.");
    }
    if (exception) {
        std::rethrow_exception(exception);
    }
}

// Take jobs and crack 'em
void PasswordCracker::consume(unsigned long i, unsigned long stop, bool metricsEnabled) {
    // We want one thread to try a few "fun" ctf passwords first
    if (i == 0 && !Args.SkipDefaultGuesses.getValue()) {
        sillyCtfGuesses();
    }
    // Next, load the wordlist file
    FILE *pWordList;
    pWordList = fopen(Args.WordlistFn.getValue().c_str(), "r");
    // Skip to our section
    fseek(pWordList, i, SEEK_SET);

    unsigned int batchProgess = 0;

    // Dedicating 512 chars should give us a max wordlength of 128 unicode
    // characters, worst case
    char line[512];
    // We allow for some overlap here (i.e. thread n might repeat a password from
    // thread n+1) The reason for this is that we don't know where the linebreaks
    // are without reading the entire file first, which would hold up (n-1)
    // threads from actually doing useful things.
    //
    // Instead, we start n threads at different points in the wordlist, possibly
    // in the middle of a password. this is fine, since the "previous" thread will
    // always repeat the first password assigned to this thread.
    //
    while (!stopped && fgets(line, sizeof(line), pWordList)) {
        // Strip \n and \r, \rn
        line[strcspn(line, "\r\n")] = 0;

        // Try extracting with this passphrase
        if (tryPassphrase(line)) {
            success = true;
            if (!Args.ContinueAfterFirstResult.getValue()) {
                // Tell the other threads that they should stop
                stopped = true;
                handleResult(line);
                break;
            }
            handleResult(line);
        }

        // Add 1, since the newline was stripped, but is present in the file size
        batchProgess += std::strlen(line) + 1;

        // Incrementing an atomic int is quite costly, so don't do it if no one
        // cares about its value
        if (metricsEnabled && batchProgess >= batchSize) {
            progress += batchProgess;
            batchProgess = 0;
        }

        // Get out current position in the wordlist, make sure it is non-negative, so
        // we are safe to cast it to unsigned in the next compare.
        long pos = ftell(pWordList);
        myassert(pos >= 0);

        // If we've overshot the end of our section (by one word), we can stop
        if ((unsigned long)ftell(pWordList) > stop) {
            break;
        }
    }

    fclose(pWordList);
}

bool PasswordCracker::tryPassphrase(const char *Passphrase) {
    // We first check if the file magic makes sense
    if (!verifyMagic(Passphrase)) {
        return false;
    }

    std::string PassphraseStr(Passphrase);

    // In case the magic checks out, we have to do a proper extract attempt.
    // This does re-do the magic, but it's rare enough that that won't matter.

    EmbData embdata(EmbData::EXTRACT, PassphraseStr);
    Selector sel(numSamples, PassphraseStr);

    unsigned long sv_idx = 0;
    while (!embdata.finished()) {
        unsigned long embvaluesrequested =
            AUtils::div_roundup<unsigned long>(embdata.getNumBitsRequested(), bitsPerEmbValue);
        if (sv_idx + (samplesPerVertex * embvaluesrequested) >= numSamples) {
            // TODO; In theory, we should error out if we hit this.
            // However, I've seen this error happen randomly on valid input files
            // so I'm leaving it as a "return false" for now
            // throw CorruptDataError (_("the stego data from standard input is too
            // short to contain the embedded data.")) ;
            return false;
        }
        BitString bits(EmbValueModulus);
        for (unsigned long i = 0; i < embvaluesrequested; i++) {
            EmbValue ev = 0;
            for (unsigned int j = 0; j < samplesPerVertex; j++, sv_idx++) {
                ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue(sel[sv_idx])) % EmbValueModulus;
            }
            bits.appendNAry(ev);
        }

        // It's possible that we find wrong passphrase for which we decrypt a valid
        // magic Statistically, this only happens once every 256^3 times, but for a
        // large wordlist, such as rockyou, this _can_ happen.
        try {
            embdata.addBits(bits);
        }
        // Catching other errors here is fine, they should be pretty rare.
        //	* decompression error
        //	* failed crc check
        catch (const SteghideError &) {
            return false;
        }
    }
    // We've found a passphrase, for which both the file magic checks out
    // and the CRC is valid. :D
    return true;
}

void PasswordCracker::extract(std::string passphrase) {
    Extractor ext(Args.StgFn.getValue(), passphrase);
    EmbData *emb = ext.extract();
    Cracker::extract(emb);
    delete emb;
}

// Try some guesses that might come up in ctf's, but won't be
// on a wordlist.
// * empty passphrase
// * filename as passphrase
// * stripped filename as passphrase
void PasswordCracker::sillyCtfGuesses() {
    std::string filename = Utils::stripDir(Args.StgFn.getValue());
    std::string strippedFilename = filename.substr(0, filename.find_last_of("."));
    const char *guesses[] = {
        "",
        filename.c_str(),
        strippedFilename.c_str(),
    };
    for (const char *line : guesses) {
        // Try extracting with this passphrase
        VerboseMessage::print("Added password guess: \"%s\".\n", line);
        if (tryPassphrase(line)) {
            success = true;
            if (!Args.ContinueAfterFirstResult.getValue()) {
                // Tell the other threads that they should stop
                stopped = true;
                handleResult(line);
                break;
            }
            handleResult(line);
        }
    }
}

void PasswordCracker::metricLine(unsigned long cur, float percentage) {
    // In accessibility mode, print a flat percentage
    if (Args.Accessible.getValue()) {
        Message::print("%.0f%%\n", percentage);
    } else {
        Message::print("Progress: %.2f%% (%s)           \r", percentage,
                       Utils::formatHRSize(cur).c_str());
    }
}

void PasswordCracker::handleResult(std::string passphrase) {
    // Get a lock, s.t. we don't print results from multiple threads at the same time
    const std::lock_guard<std::mutex> lock(handleResultMutex);
    // Make sure we only extract one result in case continue is not set
    if (Args.ContinueAfterFirstResult.getValue() || resultNum++ == 0) {
        // Re-extract the data with the confirmed passphrase.
        // This does mean we're throwing away one valid "embdata" object, but
        // that's not a bad trade-off to be able to use steghide's structure
        Message::print("Found passphrase: \"%s\"\n", passphrase.c_str());
        extract(passphrase);
    }
}

/*
 * Stegseek 0.5 - a steghide cracker
 * Copyright (C) 2020 Rick de Jager
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

#include "Cracker.h"

#include "CvrStgFile.h"
#include "EmbData.h"
#include "Extractor.h"
#include "MHashPP.h"
#include "PseudoRandomSource.h"
#include "Selector.h"
#include "Utils.h"
#include "common.h"
#include "error.h"

#include <cstring>

Cracker::Cracker() {
    // get stegofile
    if (Args.StgFn.getValue() != "") {
        VerboseMessage::print("Using stegofile \"%s\".\n", Args.StgFn.getValue().c_str());
    } else if (Args.StgFn.is_set()) {
        VerboseMessage::print("Reading stegofile from stdin.\n");
    } else {
        throw SteghideError("No stegofile specified as input.");
    }

    // get output file
    if (Args.ExtFn.is_set()) {
        if (Args.ExtFn.getValue() != "") {
            VerboseMessage::print("Using output file \"%s\".\n", Args.ExtFn.getValue().c_str());
        } else {
            VerboseMessage::print("Using stdout as output.\n");
        }
    }

    // Print threading info
    VerboseMessage::print("Running on %d threads.\n", Args.Threads.getValue());

    // Load the Stegfile
    Globs.TheCvrStgFile = CvrStgFile::readFile(Args.StgFn.getValue().c_str());

    // Look up these parameters once, to prevent function calls for each
    // passphrase attempt
    bitsPerEmbValue = AUtils::log2_ceil<unsigned short>(Globs.TheCvrStgFile->getEmbValueModulus());
    numSamples = Globs.TheCvrStgFile->getNumSamples();
    samplesPerVertex = Globs.TheCvrStgFile->getSamplesPerVertex();
    EmbValueModulus = Globs.TheCvrStgFile->getEmbValueModulus();
    embvaluesRequestedMagic =
        AUtils::div_roundup<unsigned long>(EmbData::NBitsMagic, bitsPerEmbValue);

    // Load the embedded values into a "plain" array for faster lookup
    embeddedValues = new EmbValue[numSamples];
    for (unsigned int i = 0; i < numSamples; i++) {
        embeddedValues[i] = Globs.TheCvrStgFile->getEmbeddedValue(i);
    }

    // init counters
    progress = 0;
    saveFileIndex = 0;
    resultNum = 0;
    stopped = false;
    success = false;
}

void Cracker::metrics(unsigned long maxValue) {
    // Make sure "end" is at least 1 (in case of empty wordlist)
    maxValue = std::max(maxValue, 1UL);
    // Keep track of the previous percentage we printed
    // s.t. we can filter output in accessibility mode
    float prevPercentage = -10.0f;

    while (!stopped) {
        // Copy the (atomic) progress to a temp value
        unsigned long cur = progress;
        // Only show the progress bar if we've made considerable progress.
        // Set at half a million seeds / 5 MB
        if (cur > 500000) {
            float percentage = 100.0f * ((float)cur / (float)maxValue);
            // In accessibility mode, print a flat percentage without a huge number
            if (Args.Accessible.getValue()) {
                // Only update if we've made some additional progress:
                if (percentage - prevPercentage > accessibleUpdateThreshold) {
                    metricLine(cur, percentage);
                    prevPercentage = percentage;
                    // Add some additional wait time to prevent spamming the terminal
                    std::this_thread::sleep_for(std::chrono::milliseconds(progressDeltaAccessible));
                    continue;
                }
            } else {
                metricLine(cur, percentage);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(progressDelta));
    }
    // Print a newline before returning in case the last thing we printed contained \r
    if (!Args.Accessible.getValue()) {
        Message::printRaw("\n");
    }
}

bool Cracker::verifyMagic(std::string Passphrase) {
    MHASH td = mhash_init(MHASH_MD5);
    mhash(td, Passphrase.data(), Passphrase.size());

    UWORD32 seed = 0;
    UWORD32 hash[4];
    mhash_deinit(td, hash);
    for (unsigned int i = 0; i < 4; i++) {
        seed ^= hash[i];
    }
    return verifyMagic(seed);
}

bool Cracker::verifyMagic(const char *Passphrase) {
    MHASH td = mhash_init(MHASH_MD5);
    mhash(td, Passphrase, std::strlen(Passphrase));

    UWORD32 seed = 0;
    UWORD32 hash[4];
    mhash_deinit(td, hash);
    for (unsigned int i = 0; i < 4; i++) {
        seed ^= hash[i];
    }
    return verifyMagic(seed);
}

bool Cracker::verifyMagic(UWORD32 seed) {
    // Create a buf to keep track of rng collisions
    // the size of the buffer is equal to the length of the magics array,
    // multiplied by the number of samples used to encode one vertex.
    UWORD32 rngBuf[25 * samplesPerVertex];

    if (samplesPerVertex * embvaluesRequestedMagic >= numSamples) {
        // TODO; In theory, we should error out if we hit this.
        // However, I've seen this error happen randomly on valid input files
        // so I'm leaving it as a "return false" for now
        // throw CorruptDataError (_("the stego data from standard input is too
        // short to contain the embedded data.")) ;
        return false;
    }
    EmbValue ev = 0;
    unsigned long sv_idx = 0;
    for (unsigned long i = 0; i < embvaluesRequestedMagic; i++) {
        for (unsigned int j = 0; j < samplesPerVertex; j++, sv_idx++) {
            // Calc next random number
            seed = (UWORD32)(seed * PseudoRandomSource::A + PseudoRandomSource::C);
            // Get next value index
            const UWORD32 valIdx =
                sv_idx + (((double)seed / (double)4294967296.0) * ((double)(numSamples - sv_idx)));
            // Check for RNG collisions. Should be fairly rare as numSample gets
            // larger
            for (unsigned long k = 0; k < sv_idx; k++) {
                if (rngBuf[k] == valIdx) {
                    // In case we find an rng collision, just pretend the magic check was
                    // successful. The stricter steghide extractor will double check our
                    // work
                    //
                    // This case is rare
                    return true;
                }
            }
            // Save the value index, to keep track of collisions
            rngBuf[sv_idx] = valIdx;
            ev = (ev + embeddedValues[valIdx]) % EmbValueModulus;
        }

        // Compare each bit in the retrieved embValue individually.
        for (short k = 0; k < bitsPerEmbValue; k++) {
            short currentBit = (ev >> k) & 1;
            int bitsSeen = i * bitsPerEmbValue + k;
            // Add an extra check to make sure we don't touch bit 26, even if bitsPerEmbValue > 1
            if (currentBit != ((magic >> bitsSeen) & 1) && bitsSeen < 25) {
                return false;
            }
        }
        ev = 0;
    }
    return true;
}

void Cracker::extract(EmbData *emb) {
    std::string origFn = emb->getFileName();
    std::string outFn = Utils::stripDir(Globs.TheCvrStgFile->getName()) + ".out";
    if (Args.ExtFn.is_set()) {
        outFn = Args.ExtFn.getValue();
    }

    if (origFn != "") {
        Message::print("Original filename: \"%s\".\n", origFn.c_str());
    }
    if (outFn == "") {
        Message::print("Extracting to stdout.\n\n");
    } else {
        // In case we're extracting to a file, we need to make sure this file is unique.
        // In some cases, a stego file might contain two embedded files, both of which we
        // should be able to extract.

        // Increment and copy atomically
        unsigned int fileNum = saveFileIndex++;
        // Append the fileNum to the outfile, unless this is the first file
        if (fileNum != 0) {
            outFn += "." + std::to_string(fileNum);
        }
        Message::print("Extracting to \"%s\".\n", outFn.c_str());
    }
    BinaryIO io(outFn, BinaryIO::WRITE);
    std::vector<BYTE> data = emb->getData();
    for (std::vector<BYTE>::iterator i = data.begin(); i != data.end(); i++) {
        io.write8(*i);
    }
    io.close();
}

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

#include "SeedCracker.h"

#include "CvrStgFile.h"
#include "EmbData.h"
#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"
#include "Extractor.h"
#include "Selector.h"
#include "Utils.h"
#include "common.h"
#include "error.h"

void SeedCracker::crack() {
    // Initialize threads
    std::vector<std::thread> ThreadPool;
    unsigned int threads = Args.Threads.getValue();
    bool metricsEnabled = Args.Verbosity.getValue() != QUIET;

    // Add a thread to keep track of metrics
    if (metricsEnabled) {
        ThreadPool.push_back(std::thread([this] { metrics(UWORD32_MAX, "seeds"); }));
    }

    // Add n worker threads
    unsigned int part = UWORD32_MAX / threads;
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

    // If we didn't find a passphrase, print a message directly to stderr
    if (!success) {
        fprintf(stderr, "[!] Could not find a valid seed.\n");
    } else {
        // Output the found seed. At the moment this isn't particularly
        // useful to then end-user, it's nice for debugging
        Message msg;
        msg.setMessage("[i] --> Found seed: \"%x\"", foundResult.seed);
        msg.printMessage();
        finish();
    }
}

// Take jobs and crack 'em
void SeedCracker::consume(unsigned int i, unsigned int stop, bool metricsEnabled) {
    while (!stopped && i < stop) {
        // Try extracting with this seed
        if (trySeed(i)) {
            // Tell the other threads that they should stop
            stopped = true;
            success = true;
        }
        i++;

        // Incrementing an atomic int is quite costly, so don't do it if no one
        // cares about its value
        if (metricsEnabled) {
            // Add to the perf metric
            progress++;
        }
    }
}

bool SeedCracker::trySeed(UWORD32 seed) {
    // First verify the magic
    if (!verifyMagic(seed)) {
        return false;
    }
    // Next, try to interpret the remaining bits
    // magic + enc mode + enc algo + plain size
    // We _need_ to check the magic as well, since verifyMagic
    // can throw false positives
    const unsigned int requestedValues = 25 + 3 + 5 + 32;

    // Create a "proper" selector object
    Selector sel(numSamples, seed);
    EmbValue ev = 0;
    unsigned int encAlgo = 0;
    unsigned int encMode = 0;
    unsigned int plainSize = 0;
    unsigned long sv_idx = 0;
    const int magics[25] = {// Magic, "shm" in binary LE
                            1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0,
                            // Code version (0)
                            0};

    for (unsigned int i = 0; i < requestedValues; i++) {
        for (unsigned int j = 0; j < samplesPerVertex; j++, sv_idx++) {
            ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue(sel[sv_idx])) % EmbValueModulus;
        }
        // Check magic
        if (i < 25) {
            if (magics[i] != ev) {
                return false;
            }
        }
        // Get enc algo
        if (25 <= i && i < 30) {
            encAlgo ^= ev << (i - 25);
            if (encAlgo > 22) {
                return false;
            }
        }
        // Get enc mode
        if (30 <= i && i < 33) {
            encMode ^= ev << (i - 30);
        }
        // get Plain size
        if (33 <= i && i < 65) {
            plainSize ^= ev << (i - 33);
            if (plainSize * samplesPerVertex > numSamples - requestedValues * samplesPerVertex) {
                // This plain size wouldn't fit, so the seed must be wrong.
                return false;
            }
        }
        ev = 0;
    }

    // Save the values on success
    foundResult = Result{
        seed,
        plainSize,
        encAlgo,
        encMode,
    };
    return true;
}

void SeedCracker::finish() {
    std::string size = Utils::formatHRSize(foundResult.plainSize / 8);
    // Print directly to stderr s.t. the result will even be printed in quiet
    // mode. There is no good reason to run this in quiet mode, and waiting for 5
    // minutes to realise you accidentically set -q would be annoying
    fprintf(stderr,
            "Plain size: %s (compressed)\n"
            "Encryption Algorithm: %s\n"
            "Encryption Mode:      %s\n",
            size.c_str(),
            EncryptionAlgorithm::translate(EncryptionAlgorithm::IRep(foundResult.encAlgo)).c_str(),
            EncryptionMode::translate(EncryptionMode::IRep(foundResult.encMode)).c_str());

    // Data is not encrypted :D
    // Let's dump it to a file
    if (foundResult.encAlgo == 0) {
        Extractor ext(Args.StgFn.getValue(), foundResult.seed);
        EmbData *emb = ext.extract();
        extract(emb);
        delete emb;
    }
}
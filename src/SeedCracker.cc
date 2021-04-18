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
        ThreadPool.push_back(std::thread([this] { metrics(UWORD32_MAX); }));
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

    // If we didn't find a passphrase, throw an error.
    if (!success) {
        throw SteghideError("Could not find a valid seed.");
    }
    if (exception) {
        std::rethrow_exception(exception);
    }
}

// Take jobs and crack 'em
void SeedCracker::consume(unsigned int i, unsigned int stop, bool metricsEnabled) {
    unsigned int batchProgress = 0;
    while (!stopped && i < stop) {
        // Try extracting with this seed
        Result result = trySeed(i);
        if (result.found) {
            success = true;
            if (!Args.ContinueAfterFirstResult.getValue()) {
                // Tell the other threads that they should stop
                stopped = true;
            }
            handleResult(result);
        }
        i++;
        batchProgress++;

        // Incrementing an atomic int is quite costly, so don't do it if no one
        // cares about its value
        if (metricsEnabled && batchProgress >= batchSize) {
            // Add to the perf metric
            progress += batchProgress;
            batchProgress = 0;
        }
    }
}

Result SeedCracker::trySeed(UWORD32 seed) {
    // First verify the magic
    if (!verifyMagic(seed)) {
        return Result{};
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

    for (unsigned int i = 0; i < requestedValues; i++) {
        for (unsigned int j = 0; j < samplesPerVertex; j++, sv_idx++) {
            ev = (ev + Globs.TheCvrStgFile->getEmbeddedValue(sel[sv_idx])) % EmbValueModulus;
        }
        for (short k = 0; k < bitsPerEmbValue; k++) {
            short currentBit = (ev >> k) & 1;
            int bitsSeen = i * bitsPerEmbValue + k;
            // Check magic
            if (bitsSeen < 25) {
                if (((magic >> bitsSeen) & 1) != currentBit) {
                    return Result{};
                }
            }
            // Get enc algo
            if (25 <= bitsSeen && bitsSeen < 30) {
                encAlgo ^= currentBit << (bitsSeen - 25);
                if (encAlgo > 22) {
                    return Result{};
                }
            }
            // Get enc mode
            if (30 <= bitsSeen && bitsSeen < 33) {
                encMode ^= currentBit << (bitsSeen - 30);
            }
            // get Plain size
            if (33 <= bitsSeen && bitsSeen < 65) {
                plainSize ^= currentBit << (bitsSeen - 33);
                if (plainSize * samplesPerVertex >
                    numSamples - requestedValues * samplesPerVertex) {
                    // This plain size wouldn't fit, so the seed must be wrong.
                    return Result{};
                }
            }
        }
        ev = 0;
    }

    // Save the values on success
    return Result{
        true, seed, plainSize, encAlgo, encMode,
    };
}

void SeedCracker::handleResult(Result result) {
    // Get a lock, s.t. we don't print results from multiple threads at the same time
    const std::lock_guard<std::mutex> lock(handleResultMutex);
    // Make sure we only extract one result in case continue is not set
    if (Args.ContinueAfterFirstResult.getValue() || resultNum++ == 0) {
        std::string size = Utils::formatHRSize(result.plainSize / 8);
        Message::print(
            "Found (possible) seed: \"%08x\"            \n"
            "\tPlain size: %s (compressed)\n"
            "\tEncryption Algorithm: %s\n"
            "\tEncryption Mode:      %s\n",
            result.seed, size.c_str(),
            EncryptionAlgorithm::translate(EncryptionAlgorithm::IRep(result.encAlgo)).c_str(),
            EncryptionMode::translate(EncryptionMode::IRep(result.encMode)).c_str());

        // Data is not encrypted :D
        // Let's dump it to a file
        if (result.encAlgo == 0) {
            Extractor ext(Args.StgFn.getValue(), result.seed);
            EmbData *emb = ext.extract();
            extract(emb);
            delete emb;
        }
    }
}

void SeedCracker::metricLine(unsigned long cur, float percentage) {
    // In accessibility mode, print a flat percentage
    if (Args.Accessible.getValue()) {
        Message::print("%.0f%%\n", percentage);
    } else {
        Message::print("Progress: %.2f%% (%lu seeds)           \r", percentage, cur);
    }
}

/*
 *
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

#ifndef SH_CRACKER_H
#define SH_CRACKER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <thread>

#include "EmbData.h"
#include "Selector.h"

class Cracker {
  public:
    Cracker();

    void crack();

  protected:
    void metrics(unsigned long maxValue);
    virtual void metricLine(unsigned long cur, float percentage){};
    void finish();
    void extract(EmbData *);
    bool verifyMagic(std::string);
    bool verifyMagic(const char *);
    bool verifyMagic(UWORD32);

    // Variables
    bool success;
    bool stopped;
    std::atomic<unsigned long> progress;
    std::atomic<unsigned int> saveFileIndex;
    std::atomic<unsigned int> resultNum;
    std::mutex handleResultMutex;
    std::exception_ptr exception = nullptr;

    // File properties
    unsigned short bitsPerEmbValue;
    unsigned long numSamples;
    unsigned short samplesPerVertex;
    EmbValue EmbValueModulus;
    unsigned long embvaluesRequestedMagic;

    // Constants
    // 's', 'h', 'm', 0
    const unsigned int magic = 0x073688d;
    const float accessibleUpdateThreshold = 1.0f;
    const unsigned int progressDelta = 40;
    const unsigned int progressDeltaAccessible = 500;

  private:
    EmbValue *embeddedValues;
};

#endif // ndef SH_CRACKER_H

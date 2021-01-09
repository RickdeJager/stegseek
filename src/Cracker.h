/*
 *
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

#ifndef SH_CRACKER_H
#define SH_CRACKER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
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
    void metrics(unsigned long max, const char *unit);
    void finish();
    void extract(EmbData *);
    bool verifyMagic(std::string);
    bool verifyMagic(const char *);
    bool verifyMagic(UWORD32);

    // Control variables
    bool success;
    bool stopped;
    std::atomic<unsigned long> progress;

    // File properties
    unsigned short bitsperembvalue;
    unsigned long numSamples;
    unsigned short samplesPerVertex;
    EmbValue EmbValueModulus;
    unsigned long embvaluesRequestedMagic;

  private:
    EmbValue *embeddedValues;
};

#endif // ndef SH_CRACKER_H

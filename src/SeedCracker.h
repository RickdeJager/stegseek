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

#ifndef SH_SEEDCRACKER_H
#define SH_SEEDCRACKER_H

#include <atomic>
#include <string>

#include "Cracker.h"
#include "Selector.h"

struct Result {
    unsigned int seed;
    unsigned int plainSize;
    unsigned int encAlgo;
    unsigned int encMode;
};

class SeedCracker : Cracker {
  public:
    SeedCracker(){};

    void crack();

  private:
    bool trySeed(UWORD32);
    void consume(unsigned int, unsigned int, bool);
    void finish();
    Result foundResult;
};

#endif // ndef SH_SEEDCRACKER_H

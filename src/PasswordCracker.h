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

#ifndef SH_PASSWORDCRACKER_H
#define SH_PASSWORDCRACKER_H

#include <atomic>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <thread>

#include "Cracker.h"
#include "Selector.h"

class PasswordCracker : Cracker {
  public:
    PasswordCracker();
    void crack();

  protected:
    void metricLine(unsigned long, float) override;

  private:
    bool tryPassphrase(const char *);
    void extract(std::string);
    void consume(unsigned long, unsigned long, bool);
    void handleResult(std::string);
    void sillyCtfGuesses();
    std::ifstream wordlist;
    unsigned long wordlistLength = 0;
    const unsigned int batchSize = 20000;
};

#endif // ndef SH_PASSWORDCRACKER_H

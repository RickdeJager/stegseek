/*
 *
 * Stegseek 0.3 - a steghide cracker
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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <atomic>
#include <chrono>
#include <set>

#include "Selector.h"

class SeedCracker {
	public:
	SeedCracker (void) ;

	~SeedCracker (void) ;

	void crack (void) ;

	private:
	bool trySeed (UWORD32) ;
	bool verifyMagic (UWORD32) ;
	void consume (unsigned int, unsigned int) ;
	void metrics () ;
	bool success ;
	bool stopped ;
	UWORD32 foundSeed ;
	std::atomic<unsigned int> attempts ;
	std::vector<std::queue<UWORD32>> WorkQueues ;

	unsigned short bitsperembvalue ;
	unsigned long numSamples ;
	unsigned short samplesPerVertex ;
	EmbValue EmbValueModulus ;
	unsigned long embvaluesRequestedMagic ;

} ;

#endif // ndef SH_SEEDCRACKER_H

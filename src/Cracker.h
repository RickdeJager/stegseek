/*
 *
 * [name] [version] - a steghide cracker
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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <atomic>

class Cracker {
	public:
	Cracker (void) ;

	~Cracker (void) ;

	void crack (void) ;

	private:
	bool tryPassphrase (std::string) ;
	void extract (std::string) ;
	void consume () ;
	std::ifstream wordlist ;
	std::mutex QueueMutex ;
	std::condition_variable cv ;
	std::atomic<bool> stopped ;
	std::string foundPassphrase ;
	std::queue<std::string> WorkQueue ;

	unsigned short bitsperembvalue ;
	unsigned long numSamples ;
	unsigned short samplesPerVertex ;
	EmbValue EmbValueModulus ;

} ;

#endif // ndef SH_CRACKER_H

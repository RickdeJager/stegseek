/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>
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

#include <cstdio>

#include "Utils.h"

std::string Utils::formatHRSize(unsigned long size) {
    float s = (float)size;

    std::string unit = "Byte(s)";
    if (s > 1024.0) {
        s /= 1024.0;
        unit = "KB";
    }
    if (s > 1024.0) {
        s /= 1024.0;
        unit = "MB";
    }
    if (s > 1024.0) {
        s /= 1024.0;
        unit = "GB";
    }

    char buf[15];
    sprintf(buf, "%.1f %s", s, unit.c_str());
    return std::string(buf);
}

std::string Utils::stripDir(std::string s) {
    unsigned int start = 0;
    if ((start = s.find_last_of("/\\")) == std::string::npos) {
        start = 0;
    } else {
        start += 1;
    }
    return s.substr(start, std::string::npos);
}

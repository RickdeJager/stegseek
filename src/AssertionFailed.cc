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
 * 2020:
 *  Removed original authors email from the assert message, as I've made
 * a significant amount of changes and bugs in stegseek might not be his fault.
 */

#include "AssertionFailed.h"
#include <cstring>

void AssertionFailed::printMessage() const {
    printf("\n");
    SteghideError::printMessage();
    printf(_("This means that you have found a bug. Please open an issue on Github\n"
             "if you have a way to reproduce the error.\n"
             "Stegseek has to exit now. Sorry.\n"));
}

char *AssertionFailed::stripDir(const char *fn) {
    int i = 0, j = 0, start = 0, end = 0;

    end = i = strlen(fn) - 1;
    while ((i >= 0) && (fn[i] != '\\') && (fn[i] != '/')) {
        i--;
    }
    start = i + 1;

    char *retval = (char *)malloc(end - start + 2);

    j = 0;
    for (i = start; i <= end; i++, j++) {
        retval[j] = fn[i];
    }
    retval[j] = '\0';

    return retval;
}

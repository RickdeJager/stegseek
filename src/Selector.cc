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

#include "Selector.h"
#include "BitString.h"
#include "MHashPP.h"
#include "PseudoRandomSource.h"

Selector::Selector(UWORD32 m, std::string pp) : Maximum(m), NumInArray(0) {
    UWORD32 seed = 0;
    MHASH td = mhash_init(MHASH_MD5);
    mhash(td, pp.data(), pp.size());

    UWORD32 hash[4];
    mhash_deinit(td, hash);
    for (unsigned int i = 0; i < 4; i++) {
        seed ^= hash[i];
    }
    PRandom = new PseudoRandomSource(seed);

    // Reserve some space to prevent many resizes
    X.reserve(99);
    Y.reserve(99);
}

Selector::Selector(UWORD32 m, UWORD32 seed) : Maximum(m), NumInArray(0) {
    PRandom = new PseudoRandomSource(seed);
    // Reserve some space to prevent many resizes
    X.reserve(99);
    Y.reserve(99);
}

Selector::Selector(UWORD32 m) {
    Maximum = m;
    X.resize(m);
    for (UWORD32 i = 0; i < m; i++) {
        X[i] = i;
    }
    NumInArray = m;
    PRandom = NULL;
}

Selector::Selector(const std::vector<UWORD32> &retvals) {
    X = retvals;
    Maximum = retvals.size();
    NumInArray = retvals.size();
    PRandom = NULL;
}

Selector::~Selector() {
    if (PRandom) {
        delete PRandom;
    }
}

UWORD32 Selector::operator[](UWORD32 i) {
    myassert(i < Maximum);
    calculate(i + 1);
    return X[i];
}

void Selector::calculate(UWORD32 m) {
    UWORD32 j = NumInArray;

    if (m > NumInArray) {
        NumInArray = m;
        // Never downsize the vector
        if (NumInArray > X.size()) {
            X.resize(NumInArray);
            Y.resize(NumInArray);
        }
    }

    for (; j < m; j++) {
        // k is a number from {j,...,Maximum-1}
        UWORD32 k = j + PRandom->getValue(Maximum - j);

        UWORD32 i = 0;
        // First we check whether this random number k has been seen before.
        // In case of such a collision, we use 'j' as our "random" k instead.
        //
        // I think the original idea is that j can't collide with k, because k \in
        // (j, max], However, k is actually \in [j, max]. It's still very unlikely
        // to collide, so ¯\_(ツ)_/¯
        //
        if (idxX(k, j, &i)) { // there exists i < j with X[i] = k
            // use Y[i] instead of k
            setX(j, Y[i]);

            if (X[j] > j) { // some future k could be X[j]
                Y[j] = j;
            }
            if (X[i] > j) { // some future k could be X[i]
                Y[i] = j;
                UWORD32 l = 0;
                if (idxX(Y[i], j, &l)) {
                    Y[i] = Y[l];
                }
            }
        } else {
            setX(j, k);
            Y[j] = j;
        }

        if (X[j] > j) {
            UWORD32 i = 0;
            if (idxX(Y[j], j, &i)) {
                Y[j] = Y[i];
            }
        }
    }
}

// This function tries to find an index in X, such that X[index] = v.
// The index must be in the range [0..m]
//
// It is mainly used to check for collisions with previously generated random
// numbers
//
// Random number, Index j, Pointer to i
bool Selector::idxX(UWORD32 v, UWORD32 m, UWORD32 *p) const {
    bool retval = false;
    std::unordered_map<UWORD32, UWORD32>::const_iterator it = Xreversed.find(v);
    if ((it != Xreversed.end()) && (it->second < m)) {
        *p = it->second;
        retval = true;
    }
    return retval;
}

void Selector::setX(UWORD32 i, UWORD32 v) {
    X[i] = v;
    Xreversed[v] = i;
}
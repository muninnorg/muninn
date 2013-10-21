// random_utils.h
// Copyright (c) 2010-2012 Jes Frellsen
//
// This file is part of Muninn.
//
// Muninn is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// Muninn is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Muninn.  If not, see <http://www.gnu.org/licenses/>.
//
// The following additional terms apply to the Muninn software:
// Neither the names of its contributors nor the names of the
// organizations they are, or have been, associated with may be used
// to endorse or promote products derived from this software without
// specific prior written permission.

#ifndef RANDOM_UTILS_H_
#define RANDOM_UTILS_H_

#include <cstdlib>

/// Primitive function for getting a random number between 0 and 1
///
/// \return A random number between 0 and 1.
double inline randomD() {
    return 1.0 * rand() / RAND_MAX;
}

/// Primitive function for generating a random integer in the interval [0;n).
///
/// \param i The maximal value of the random integer.
/// \return A random integer.
static int inline randomI(unsigned int n) {
    return rand() % n;
}

#endif // RANDOM_UTILS_H_

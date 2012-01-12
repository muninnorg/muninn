// TArrayUtils.cpp
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

#include "muninn/utils/TArray.h"

namespace Muninn {

void copy_to_flat(DArray &from, DArray &to, BArray &where) {
  unsigned int size = from.get_asize();
  double *fromPtr = from.get_array();
  double *toPtr = to.get_array();
  bool *wherePtr = where.get_array();

  unsigned int j=0;
  for(unsigned int i=0; i<size; i++) {
    if(wherePtr[i]) {
      toPtr[j] = fromPtr[i];
      j++;
    }
  }
}

void copy_from_flat(DArray &from, DArray &to, BArray &where) {
  unsigned int size = to.get_asize();
  double *fromPtr = from.get_array();
  double *toPtr = to.get_array();
  bool *wherePtr = where.get_array();

  unsigned int j=0;
  for(unsigned int i=0; i<size; i++) {
    if(wherePtr[i]) {
      toPtr[i] = fromPtr[j];
      j++;
    }
  }
}

void copy_to_flat_negate(DArray &from, DArray &to, BArray &where) {
  unsigned int size = from.get_asize();
  double *fromPtr = from.get_array();
  double *toPtr = to.get_array();
  bool *wherePtr = where.get_array();

  unsigned int j=0;
  for(unsigned int i=0; i<size; i++) {
    if(wherePtr[i]) {
      toPtr[j] = -fromPtr[i];
      j++;
    }
  }
}

void copy_from_flat_negate(DArray &from, DArray &to, BArray &where) {
  unsigned int size = to.get_asize();
  double *fromPtr = from.get_array();
  double *toPtr = to.get_array();
  bool *wherePtr = where.get_array();

  unsigned int j=0;
  for(unsigned int i=0; i<size; i++) {
    if(wherePtr[i]) {
      toPtr[i] = -fromPtr[j];
      j++;
    }
  }
}

} // namespace Muninn

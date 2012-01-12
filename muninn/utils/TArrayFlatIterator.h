// TArrayFlatIterator.h
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

#ifndef MUNINN_TARRAY_FLAT_ITERATOR_H_
#define MUNINN_TARRAY_FLAT_ITERATOR_H_

#include <iterator>

#include "muninn/common.h"
#include "muninn/utils/TArrayBaseIterator.h"

namespace Muninn {

/// A iterator that iterators over a TArray as if it was a flat array.
///
/// \tparam TARRAY The type of the TArray to iterator over.
/// \tparam T The type of the contents of the TArray.
template<typename TARRAY, typename T>
class TArrayFlatIterator : public TArrayBaseIterator<TARRAY,T> {
public:

    /// Default constructor. Note, that the class uses default copy-constructor
    /// and assignment operator.
    TArrayFlatIterator() :
        TArrayBaseIterator<TARRAY,T>() {}

    /// Construct an iterator over a given array and with a given starting index.
    ///
    /// \param array The array to iterator over.
    /// \param index The starting index in the array.
    TArrayFlatIterator(TARRAY& array, unsigned int index=0) :
        TArrayBaseIterator<TARRAY,T>(array, index) {}

    /// Incremental prefix operator for the iterator.
    ///
    /// \return A reference to the incremented iterator.
    inline TArrayFlatIterator<TARRAY,T>& operator++() {
        ++(this->index);
        return *this;
    }

    /// Evaluates if the iterator is valid and has not reached the end of the
    /// array.
    ///
    /// \return Returns false, if the end of the array is reached.
    inline bool operator()(void) {
        return this->index < this->array->get_asize();
    }
};

} // namespace Muninn

#endif /* MUNINN_TARRAY_FLAT_ITERATOR_H_ */

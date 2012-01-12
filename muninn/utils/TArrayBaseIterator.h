// TArrayBaseIterator.h
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

#ifndef MUNINN_TARRAY_BASE_ITERATOR_H_
#define MUNINN_TARRAY_BASE_ITERATOR_H_

#include <iterator>

#include "muninn/common.h"

namespace Muninn {

/// Base class for TArray iterators
///
/// \tparam TARRAY The type of the TArray to iterator over.
/// \tparam T The type of the contents of the TArray.
template<typename TARRAY, typename T>
class TArrayBaseIterator {
public:
    /// Default constructor. Note, that the class uses default copy-constructor
    /// and assignment operator.
    TArrayBaseIterator() :
        array(NULL), index(0) {}

    /// Construct an iterator over a given array and with a given starting index.
    ///
    /// \param array The array to iterator over.
    /// \param index The starting index.
    TArrayBaseIterator(TARRAY& array, unsigned int index=0) :
        array(&array), index(index) {}

    /// The dereference operator for the iterator.
    ///
    /// \return A reference to the current element.
    inline T& operator*() {
        return (*array)(index);
    }

    /// Structure Structure dereference operator for the iterator.
    ///
    /// \return A pointer to the current element.
    inline T* operator->() {
        return &((*array)(index));
    }

    /// Get the current index of the iterator.
    ///
    /// \return A reference to the current index of the iterator.
    inline const unsigned int& get_index() const {
        return index;
    }

protected:
    TARRAY* array;       ///< The array to iterator over.
    unsigned int index;  ///< The current index of the iterator.
};

} // namespace Muninn

#endif /* MUNINN_TARRAY_BASE_ITERATOR_H_ */

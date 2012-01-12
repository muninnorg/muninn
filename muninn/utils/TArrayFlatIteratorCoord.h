// TArrayFlatIteratorCoord.h
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

#ifndef MUNINN_TARRAY_FLAT_ITERATOR_CORD_H_
#define MUNINN_TARRAY_FLAT_ITERATOR_CORD_H_

#include <iterator>

#include "muninn/common.h"
#include "muninn/utils/TArrayFlatIterator.h"

namespace Muninn {

/// A iterator that iterators over a TArray as if it was a flat array. In this
/// iterator the coordinate of the current index is accessible.
///
/// \tparam TARRAY The type of the TArray to iterator over.
/// \tparam T The type of the contents of the TArray.
template<typename TARRAY, typename T>
class TArrayFlatIteratorCoord : public TArrayBaseIterator<TARRAY,T> {
public:

    /// Construct an iterator over a given array.
    ///
    /// \param array The array to iterator over.
    TArrayFlatIteratorCoord(TARRAY& array) : TArrayBaseIterator<TARRAY,T>(array), coord(array.get_ndims(), 0), depth(0)  {}

    /// Incremental prefix operator for the iterator.
    ///
    /// \return A reference to the incremented iterator.
    inline TArrayFlatIteratorCoord<TARRAY,T>& operator++() {
        this->index++;
        increment_coord();
        return *this;
    }

    /// Evaluates if the iterator is valid and has not reached the end of the
    /// array.
    ///
    /// \return Returns false, if the end of the array is reached.
    inline bool operator()(void) {
        return this->index < this->array->get_asize();
    }

    /// Get the coordinate of the current index.
    ///
    /// \return The coordinate of the current index.
    inline std::vector<unsigned int>& get_coord() {
        return coord;
    }

    /// Return the dimension number for the right most index that was changed
    /// in the last incrementation. This is called the depth of the
    /// incrementation.
    ///
    /// \return The depth of the last incrementation.
    inline unsigned int get_depth() {
        return depth;
    }

private:
    std::vector<unsigned int> coord; ///< The coordinate corresponding to the current index.
    unsigned int depth;              ///< The depth (in terms of dimensions) of the last incrementation.

    /// Increment the current coordinate by one.
    inline void increment_coord() {
        for(depth=0; depth<coord.size(); depth++) {
            coord[depth]++;
            if(coord[depth] < this->array->get_shape(depth))
                break;
            else
                coord[depth] = 0;
        }
    }
};

} // namespace Muninn

#endif /* MUNINN_TARRAY_FLAT_ITERATOR_CORD_H_ */

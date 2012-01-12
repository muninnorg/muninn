// TArrayWhereTrueIterator.h
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

#ifndef MUNINN_TARRAY_WHERE_TRUE_ITERATOR_H_
#define MUNINN_TARRAY_WHERE_TRUE_ITERATOR_H_

#include <iterator>

#include "muninn/common.h"
#include "muninn/utils/TArrayBaseIterator.h"

namespace Muninn {

/// A iterator for iterating over the elements that evaluates to true in a TArray.
///
/// \tparam TARRAY The type of the TArray to iterator over.
/// \tparam T The type of the contents of the TArray.
template<typename TARRAY, typename T>
class TArrayWhereTrueIterator : public TArrayBaseIterator<TARRAY,T>  {
public:

    /// Construct an iterator over a given array. Note that the bool values
    /// of elements in the array is evaluated upon construction of the
    /// iterator.
    ///
    /// \param array The array to iterator over.
    TArrayWhereTrueIterator(TARRAY& array) : TArrayBaseIterator<TARRAY,T>(array) {
        // Store the indicies where array are true
        for(TArrayFlatIterator<const TARRAY,const T> it = array.get_constflatiterator(); it(); ++it)
            if(*it)
                indicies.push_back(it.get_index());

        // Make an iterator over these indicies and set index to the current value
        it_indicies = indicies.begin();
        if (it_indicies < indicies.end())
            this->index = *it_indicies;
    }

    /// Incremental prefix operator for the iterator.
    ///
    /// \return A reference to the incremented iterator.
    inline TArrayWhereTrueIterator<TARRAY,T>& operator++() {
        ++it_indicies;
        // Update index, if the end hasn't been passed
        if (it_indicies < indicies.end())
            this->index = *it_indicies;
        else
            this->index = std::numeric_limits<unsigned int>::max();
        return *this;
    }

    /// Evaluates if the iterator is valid and has not reached the end.
    ///
    /// \return Returns false if the end of values has been reached.
    inline bool operator()(void) {
        return it_indicies < indicies.end();
    }

protected:
    std::vector<unsigned int> indicies;               ///< The indices of elements in the array that are true.
    std::vector<unsigned int>::iterator it_indicies;  ///< An iterator over the indicies.
};

} // namespace Muninn

#endif /* MUNINN_TARRAY_WHERE_TRUE_ITERATOR_H_ */

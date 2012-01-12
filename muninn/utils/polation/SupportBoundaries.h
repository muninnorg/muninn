// SupportBoundaries.h
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

#ifndef MUNINN_SUPPORTBOUNDARIES_H_
#define MUNINN_SUPPORTBOUNDARIES_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Exceptions/MessageException.h"

#include <vector>
#include <utility>

namespace Muninn {
namespace MLEutils {

/// Functions for finding the boundaries for boolean array i 1D.
class SupportBoundaries1D {
public:

    /// Find the left boundary on a boolean array. The left boundary is defined
    /// as lowest index, where the array value is true.
    ///
    /// \param support The array to find the boundary on.
    /// \return The index of the left boundary.
    static inline unsigned int find_left_bound(const BArray &support) {
        // Find the left side of the support
         BArray::constflatiterator it = support.get_constflatiterator();
         while (it() && !(*it)) {
             ++it;
         }

         // Assert that some support was found
         if (!it()) {
             throw MessageException("No support found on left side.");
         }

         return it.get_index();
    }

    /// Find the right boundary on a boolean array. The right boundary is defined
    /// as highest index, where the array value is true.
    ///
    /// \param support The array to find the boundary on.
    /// \return The index of the right boundary.
    static inline unsigned int find_right_bound(const BArray &support) {
        // Find the right side of the support
         BArray::constreverseflatiterator it = support.get_constreverseflatiterator();
         while (it() && !(*it)) {
             ++it;
         }

         // Assert that some support was found
         if (!it()) {
             throw MessageException("No support found on right side.");
         }

         return it.get_index();
    }

    /// Find all contiguous intervals in a boolean array, where the value of
    /// of the elements are false.
    ///
    /// \param support The array to find the intervals in.
    /// \param bounds The first and last index (where the value is false) of
    ///         the intervals are added to this vector.
    static inline void find_internal_unsupported(const BArray &support, std::vector<std::pair<unsigned int,unsigned int> > &bounds) {
        bounds.clear();

        // Find the bin bounds of the internal parts without support
        int previous = -1;

        for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
            if (it.get_index() != static_cast<unsigned int>(previous+1) && previous != -1)
                bounds.push_back(std::pair<unsigned int,unsigned int>(previous, it.get_index()));
            previous = it.get_index();
        }
    }
};

} // namespace Muninn
} // namespace MLEutils

#endif /* MUNINN_SUPPORTBOUNDARIES_H_ */

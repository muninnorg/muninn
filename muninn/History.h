// History.h
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

#ifndef MUNINN_HISTORY_H_
#define MUNINN_HISTORY_H_

#include <vector>

#include "muninn/common.h"
#include "muninn/Histogram.h"

namespace Muninn {

/// The History abstract class is the interface for classes implementing the
/// history of histograms. The history of a simulation is stored in an History
/// objects. The type of the History object may depend on the Estimator class.
class History {
public:

    /// Default virtual destructor.
    virtual ~History() {};

    /// Function for adding a histogram to the history.
    ///
    /// \param histogram The histogram to be added.
    virtual void add_histogram(const Histogram &histogram) = 0;

    /// Function for extending the shape of the History.
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    virtual void extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
        shape = add_vectors(shape, add_under, add_over);
    }

    /// Get the shape of the history.
    ///
    /// \return The shape of the history.
    inline const std::vector<unsigned int>& get_shape() const {
        return shape;
    }

    /// Remove the newest histogram from the history
    virtual void remove_newest() = 0;

protected:
    /// Construct that sets the shape of the history.
    ///
    /// \param shape The initial shape of the history
    History(const std::vector<unsigned int> &shape) : shape(shape) {};

    std::vector<unsigned int> shape; ///< The shape of the history.
};

} // namespace Muninn

#endif /* MUNINN_HISTORY_H_ */

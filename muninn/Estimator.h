// Estimator.h
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

#ifndef MUNINN_ESTIMATOR_H_
#define MUNINN_ESTIMATOR_H_

#include <vector>
#include <exception>

#include "muninn/common.h"
#include "muninn/Estimate.h"
#include "muninn/History.h"
#include "muninn/Binner.h"

namespace Muninn {

/// The estimator exception base class
class EstimatorException : public MuninnException {};

/// The Estimator base class. An Estimator can estimate the entropy (as an
/// Estimate object) based on an History and a previous Estimate.
class Estimator {
public:
    /// Virtual destructor.
    virtual ~Estimator() {}

    /// Based on a given History and a previous Estimate the function
    /// updates the Estimate.
    ///
    /// \param history The history to base the estimate on.
    /// \param estimate An old estimate that will be updated to a new
    ///                 estiamte based on the history.
    /// \param binner If a Binner has been use, it can be passed to the estimator.
    virtual void estimate(const History &history, Estimate &estimate, const Binner *binner=NULL) = 0;

    /// This function is used to extend a current estimate. The function is
    /// allowed to make use of the current history.
    ///
    /// \param extended_history The current history, which is assumed to
    ///                         already have been extended.
    /// \param estimate The current estimate to be extended.
    /// \param add_under The number of bins to be added in the lower end of
    ///                  the axis.
    /// \param add_over The number of bins to be added in the upper end of
    ///                  the axis.
    virtual void extend_estimate(const History &extended_history, Estimate &estimate, const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) = 0;

    /// Make a new empty Histogram compatible with the Estimator. The
    /// returned history may be a class derived from the Histogram base class.
    ///
    /// \param shape The shape of the new Histogram,
    /// \return A new empty Histogram.
    virtual Histogram* new_histogram(const std::vector<unsigned int> &shape) = 0;

    /// Make a new empty Histogram compatible with the Estimator based on a set
    /// of weights. The count histogram will be empty, but the histogram will
    /// get the same shape as the weights.
    ///
    /// \param lnw The weights the histogram will be initialized with.
    /// \return A new empty Histogram.
    virtual Histogram* new_histogram(const DArray &lnw) = 0;

    /// Make a new empty History that is compatible with the Estimator. The
    /// returned history may be a class derived from the History base class.
    ///
    /// \param shape The shape of the new History.
    /// \return A new empty History.
    virtual History* new_history(const std::vector<unsigned int> &shape) = 0;

    /// Make a new empty Estimate that is compatible with the Estimator. The
    /// returned Estimate may be a class derived from the Estimate base class.
    ///
    /// \param shape The shape of the new Estimate.
    /// \return A new empty Estimate.
    virtual Estimate* new_estimate(const std::vector<unsigned int> &shape) = 0;

protected:
    /// The constructor is protected since this is a abstract class with no
    /// implementations.
    Estimator () {}
};

} // namespace Muninn

#endif // MUNINN_ESTIMATOR_H_

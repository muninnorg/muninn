// MLEestimate.h
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

#ifndef MUNINN_DIFFUSION_ESTIMATE_H_
#define MUNINN_DIFFUSION_ESTIMATE_H_

#include <vector>
#include <map>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimate.h"
#include "muninn/utils/BaseConverter.h"

#include "muninn/MLE/MLEestimate.h"

namespace Muninn {

/// The estimate of the entropy (lnG) returned by the MLE class.
class DiffusionEstimate : public Estimate, public BaseConverter<Estimate, DiffusionEstimate> {
public:
    /// Construct an empty estimate with a given shape.
    ///
    /// \param shape The shape of the empty estimate.
    DiffusionEstimate(const std::vector<unsigned int> &shape) : Estimate(shape), mle_estimate(shape) {}

    /// Empty virtual destructor
    virtual ~DiffusionEstimate() {}

    // Extending the Estimator interface (see base class for documentation).
    virtual void extend(const std::vector<Index> &add_under, const std::vector<Index> &add_over) {
        Estimate::extend(add_under, add_over);
        mle_estimate.extend(add_under, add_over);
    }

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const {
        Estimate::add_statistics_to_log(statistics_logger);
        // To P&W: You should add some code here
    }

    friend class DiffusionEstimator;

private:
    MLEestimate mle_estimate;    ///< Used for storing the MLE estimate
};

} // namespace Muninn

#endif /* MUNINN_DIFFUSION_ESTIMATE_H_ */

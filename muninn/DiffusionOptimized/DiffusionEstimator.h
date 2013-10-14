// Copyright (c) 2010-2012 Jes Frellsen
// DiffusionEstimator.h
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

#ifndef MUNINN_DIFFUSION_ESTIMATOR_H_
#define MUNINN_DIFFUSION_ESTIMATOR_H_

#include <deque>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimator.h"
#include "muninn/Histories/MultiHistogramHistory.h"

#include "muninn/MLE/MLE.h"

#include "muninn/DiffusionOptimized/DiffusionHistogram.h"
#include "muninn/DiffusionOptimized/DiffusionEstimate.h"

namespace Muninn {

class DiffusionEstimator: public Estimator {
public:
    /// Constructor for the DiffusionEstimator class.
    DiffusionEstimator(Count min_count=30, unsigned int memory=20, bool restricted_individual_support=false,
            MultiHistogramHistory::HistoryMode history_mode=MultiHistogramHistory::DROP_OLDEST) :
            min_count(min_count), memory(memory), restricted_individual_support(restricted_individual_support),
            history_mode(history_mode), mle(min_count, memory, restricted_individual_support, history_mode) {}

    virtual ~DiffusionEstimator() {}

    // Implementation of Estimator interface (see base class for documentation).
    virtual void estimate(const History &history, Estimate &estimate, const Binner *binner=NULL);

    // Implementation of Estimator interface (see base class for documentation).
    virtual void extend_estimate(const History &extended_history, Estimate &estimate, const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over);

    // Implementation of Estimator interface (see base class for documentation).
    virtual Histogram* new_histogram(const std::vector<unsigned int> &shape) {
        return new DiffusionHistogram(shape);
    }

    // Implementation of Estimator interface (see base class for documentation).
    virtual Histogram* new_histogram(const DArray &lnw) {
        return new DiffusionHistogram(lnw);
    }

    // Implementation of Estimator interface (see base class for documentation).
    virtual History* new_history(const std::vector<unsigned int> &shape) {
        return new MultiHistogramHistory(shape, memory, min_count, history_mode);
    }

    // Implementation of Estimator interface (see base class for documentation).
    virtual Estimate* new_estimate(const std::vector<unsigned int> &shape) {
        return new DiffusionEstimate(shape);
    }

private:
    const Count min_count;                           ///< The minimal number of counts in a bin in order to have support in a bin.
    const unsigned int memory;                       ///< The maximal number of histogram to keep in the history.
    bool restricted_individual_support;              ///< Restrict the support of the individual histograms to only cover the support for the individual histogram.
    MultiHistogramHistory::HistoryMode history_mode; ///< Describes the procedure for deleting old histograms.
    MLE mle;                                         ///< An estimator to obtain a MLE of lnG
};

/// Exception to be thrown if the DiffusionEstimator fails
class DiffusionEstimatorException : public EstimatorException {
    virtual const char* what() const throw() {
        return "DiffusionEstimator estimation failed due to ????.";
    }
};

} // namespace Muninn

#endif // MUNINN_DIFFUSION_ESTIMATOR_H_

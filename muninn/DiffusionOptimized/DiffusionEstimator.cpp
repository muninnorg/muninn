// DiffusionEstimator.cpp
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

#include "muninn/DiffusionOptimized/DiffusionEstimator.h"

namespace Muninn {

void DiffusionEstimator::estimate(const History &base_history, Estimate &base_estimate, const Binner *binner) {
    // Check that the history and the estimate has the same shape
    assert(vector_equal(base_history.get_shape(), base_estimate.get_shape()));

    // Cast the history and estimate to be the diffusion type
    const MultiHistogramHistory& history = MultiHistogramHistory::cast_from_base(base_history, "The DiffusionEstimator is only compatible with the MultiHistogramHistory.");
    DiffusionEstimate& estimate = DiffusionEstimate::cast_from_base(base_estimate, "The DiffusionEstimator is only compatible with the DiffusionEstimate.");

    // First obtain the MLE estimate of lnG
    try {
        mle.estimate(history, estimate.mle_estimate, binner);
    }
    catch (EstimatorException &exception){
        // To P&W: You probably don't need to do anything if it fails,
        // but for now I'll rethrow the exception
        throw;
    }

    // To P&W: For now I just copy the estimates
    estimate.set_lnG(estimate.mle_estimate.get_lnG());
    estimate.set_lnG_support(estimate.mle_estimate.get_lnG_support());
    estimate.set_x0(estimate.mle_estimate.get_x0());

    // To P&W: Here you should do something with the first histogram
    assert(history.get_size() > 0);
    std::cout << history.get_size() << std::endl;
    unsigned int size = history.get_size();
    const DiffusionHistogram newest_histogram = DiffusionHistogram::cast_from_base(history[0], "The Diffusion Estimate is only compatible with DiffusionHistogram)");

    const CArray& N = newest_histogram.get_N();
    const DArray& lnw = newest_histogram.get_lnw();
}

void DiffusionEstimator::extend_estimate(const History &extended_history, Estimate &estimate, const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
    estimate.extend(add_under, add_over);
}

} // namespace Muninn

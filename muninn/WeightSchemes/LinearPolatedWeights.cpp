// LinearPolatedWeights.cpp
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

#include "muninn/WeightSchemes/LinearPolatedWeights.h"

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/Estimate.h"
#include "muninn/History.h"
#include "muninn/Histories/MultiHistogramHistory.h"

#include "muninn/utils/polation/LinearPolator.h"
#include "muninn/utils/polation/LinearPolator1dUniform.h"

#include "muninn/tools/CanonicalProperties.h"

namespace Muninn {

DArray LinearPolatedWeigths::get_weights(const Estimate &estimate, const History &base_history, const Binner *binner) {
    // Get the weights from the underlying weight scheme
    DArray weights = underlying_weight_scheme->get_weights(estimate, base_history, binner);

    // Cast the history to be a MultiHistogramHistory
    const MultiHistogramHistory& history = MultiHistogramHistory::cast_from_base(base_history, "The LinearPolatedWeigths weight scheme is only compatible with an estimator that uses a MultiHistogramHistory.");

    // Check if there is any support, if not the extrapolation scheme set all weights uniformly
    if (number_of_true(estimate.get_lnG_support()) == 0) {
        weights = 0.0;
    }
    else {
        // Calculate the sum of the counts in each bin
        const CArray &sum_N = history.get_sum_N();

        // Calculate min and max slope
        double min_slope = -max_beta_extrapolation;
        double max_slope = -min_beta_extrapolation;

        // Do the linear extrapolation and interpolation
        if (binner && !binner->is_uniform()) {
            DArray bin_centers = binner->get_binning_centered();

            MLEutils::LinearPolator1d<DArray> linear_polator(weights, estimate.get_lnG_support(), sum_N, bin_centers, sigma);
            extrapolation_details = linear_polator.extrapolate(slope_factor_up, slope_factor_down, min_slope, max_slope, min_slope, max_slope);
            linear_polator.interpolate();

            // Cache the bound centers to be used in get_extrapolated_weight
            left_bound_center = bin_centers(extrapolation_details.first.first);
            right_bound_center = bin_centers(extrapolation_details.second.first);
        }
        else {
            MLEutils::LinearPolator1dUniform linear_polator(weights, estimate.get_lnG_support(), sum_N, sigma);
            extrapolation_details = linear_polator.extrapolate(slope_factor_up, slope_factor_down, min_slope, max_slope, min_slope, max_slope);
            linear_polator.interpolate();

            // Cache the bound centers to be used in get_extrapolated_weight
            if (binner) {
                DArray bin_centers = binner->get_binning_centered();
                left_bound_center = bin_centers(extrapolation_details.first.first);
                right_bound_center = bin_centers(extrapolation_details.second.first);
            }
        }

        // Limit the slope of the weight function based on thermodynamic beta limits
        if ((std::isfinite(min_beta_thermodynamics) || std::isfinite(max_beta_thermodynamics)) && binner) {
            // Setup the object for calculating canonical properties
            DArray bin_centers = binner->get_binning_centered();
            CanonicalProperties canonical_properties(bin_centers, estimate.get_lnG(), estimate.get_lnG_support());

            // Look at the left bound
            if (std::isfinite(max_beta_thermodynamics)) {
                // Find the average energy and bin corresponding to the given beta
                double energy = canonical_properties.E(max_beta_thermodynamics);
                int bin0 = binner->calc_bin(energy);

                // Check that the bin lies at least sigma support bins for the bounds
                if (has_min_support_left(sigma, bin0, estimate.get_lnG_support()) && has_min_support_right(sigma, bin0, estimate.get_lnG_support())) {
                    // Set the maximal slope left of bin0 to max_beta_thermodynamics
                    double accumulated_offset = 0.0;

                    for (int bin=bin0; 0<bin; --bin) {
                        int bin1 = bin-1;

                        // Adjust the weight in bin1 with the accumulated offset
                        weights(bin1) += accumulated_offset;

                        // Check that the slope for bin1 to bin is not above max_beta_thermodynamics
                        // If the slope over (bin1, bin) is more than max_beta_thermodynamics then set it to max_beta_thermodynamics
                        double alpha = (weights(bin1)-weights(bin)) / (bin_centers(bin1)-bin_centers(bin));

                        if (-alpha > max_beta_thermodynamics) {
                            double new_weight_bin1 = -max_beta_thermodynamics*(bin_centers(bin1)-bin_centers(bin)) + weights(bin);
                            double offset = new_weight_bin1 - weights(bin1);

                            weights(bin1) += offset;
                            accumulated_offset += offset;
                         }
                    }

                    // Check if the the extrapolation_details should be updated
                    if (-extrapolation_details.first.second > max_beta_thermodynamics) {
                        extrapolation_details.first.second = -max_beta_thermodynamics;
                    }
                }
            }

            // Look at the right bound
            if (std::isfinite(min_beta_thermodynamics)) {
                // Find the average energy and bin corresponding to the given beta
                double energy = canonical_properties.E(min_beta_thermodynamics);
                int bin0 = binner->calc_bin(energy);

                // Check that the bin lies at least sigma support bins for the bounds
                if (has_min_support_left(sigma, bin0, estimate.get_lnG_support()) && has_min_support_right(sigma, bin0, estimate.get_lnG_support())) {
                    // Set the minimal slope right of bin0 to min_beta_thermodynamics
                    double accumulated_offset = 0.0;

                    for (int bin=bin0; bin<(static_cast<int>(weights.get_asize())-1); ++bin) {
                        int bin1 = bin+1;

                        // Adjust the weight in bin1 with the accumulated offset
                        weights(bin1) += accumulated_offset;

                        // Check that the slope for bin1 to bin is not below min_beta_thermodynamics.
                        // If the slope over (bin, bin1) is less than min_beta_thermodynamics then set it to min_beta_thermodynamics
                        double alpha = (weights(bin1)-weights(bin)) / (bin_centers(bin1)-bin_centers(bin));

                        if (-alpha < min_beta_thermodynamics) {
                            double new_weight_bin1 = -min_beta_thermodynamics*(bin_centers(bin1)-bin_centers(bin)) + weights(bin);
                            double offset = new_weight_bin1 - weights(bin1);

                            weights(bin1) += offset;
                            accumulated_offset += offset;
                         }
                    }

                    // Check if the the extrapolation_details should be updated
                    if (-extrapolation_details.second.second < min_beta_thermodynamics) {
                        extrapolation_details.second.second = -min_beta_thermodynamics;
                    }
                }
            }
        }
    }
    return weights;
}

double LinearPolatedWeigths::get_extrapolated_weight(double value, const DArray &lnw, const Estimate &estimate, const History &history, const Binner &binner) {
    int bin = binner.calc_bin(value);

    if (bin < 0) {
        unsigned int &bin0 = extrapolation_details.first.first;
        double &slope = extrapolation_details.first.second;
        return lnw(bin0) + slope*(value - left_bound_center);
    }
    else if (static_cast<int>(binner.get_nbins())<=bin) {
        unsigned int &bin0 = extrapolation_details.second.first;
        double &slope = extrapolation_details.second.second;
        return lnw(bin0) + slope*(value - right_bound_center);
    }
    else {
        throw MessageException("LinearPolatedWeigths::get_extrapolated_weight can only be used outside the binned area!");
    }
}

bool LinearPolatedWeigths::has_min_support_left(unsigned int min_support, unsigned int bin0, const BArray& support) {
    unsigned int nsupport = 0;
    int bin = bin0;
    while(0<=bin && nsupport<min_support) {
        if(support(bin)) {
            ++nsupport;
        }
        --bin;
    }
    return (nsupport>=min_support);
}

bool LinearPolatedWeigths::has_min_support_right(unsigned int min_support, unsigned int bin0, const BArray& support) {
    unsigned int nsupport = 0;
    int bin = bin0;
    while(bin<static_cast<int>(support.get_asize()) && nsupport<min_support) {
        if(support(bin)) {
            ++nsupport;
        }
        ++bin;
    }
    return (nsupport>=min_support);
}

} // namespace Muninn

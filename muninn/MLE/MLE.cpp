// MLE.cpp
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

#include <limits>

#include "muninn/MLE/MLE.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/utils/TArrayMath.h"
#include "muninn/utils/nonlinear/NonlinearEquation.h"
#include "muninn/utils/nonlinear/newton.h"

#include "muninn/utils/polation/SupportBoundaries.h"
#include "muninn/utils/polation/AverageSlope.h"
#include "muninn/utils/polation/AverageSlope1dUniform.h"

#include "utils/GMHequations.h"
#include "utils/GMHequationsAccumulated.h"

namespace Muninn {

void MLE::estimate(const History &base_history, Estimate &base_estimate, const Binner *binner) {
    // Check that the history and the estimate has the same shape
    assert(vector_equal(base_history.get_shape(), base_estimate.get_shape()));

    // Cast the history and estimate to be the MLE type
    const MultiHistogramHistory& history = MultiHistogramHistory::cast_from_base(base_history, "The MLE estimator is only compatible with the MultiHistogramHistory.");
    MLEestimate& estimate = MLEestimate::cast_from_base(base_estimate, "The MLE estimator is only compatible with the MLEestimate.");

    // Calculate the sum of the counts in each bin
    const CArray &sum_N = history.get_sum_N();

    // Calculate the bins where we have support
    BArray lnG_support = (sum_N>=min_count);

    // Check if there is any support
    if (number_of_true(lnG_support) == 0) {
        MessageLogger::get().warning("The MLE entropy cannot be estimated, since the histograms does not have any support.");
        estimate.set_lnG(0.0);
        estimate.set_lnG_support(false);
    }
    else {
        // Calculate the total number of counts in each histogram, but only where we have support
        CArray support_n(history.get_size());
        for (unsigned int set=0; set<history.get_size(); set++)
            support_n(set) = history[set].get_N().sum(lnG_support);

        // Make an array of free energies from the free energies in the previous estimate
        DArray free_energies(history.get_size());

        for (unsigned int set=0; set<history.get_size(); set++) {
            if (estimate.free_energies.count(&history[set]) > 0) {
                free_energies(set) = estimate.free_energies[&history[set]];
            }
            else if (set > 0) {
                // TODO: Do something if the free energy hasn't previous been estimated?
                MessageLogger::get().warning("Missing previous MLE entropy for histogram number " + to_string<unsigned int>(set) + " in the history.");
            }
        }

        // Check if the reference bin is set in the estimate and it has support
        if (estimate.get_x0().size()==0 || sum_N(estimate.get_x0())<min_count) {
            std::vector<unsigned int> new_x0 = arg_max(sum_N);

            if (estimate.get_x0().size()!=0) {
                MessageLogger::get().info("Moving x0 from " + to_string(estimate.get_x0()) + " to " + to_string(new_x0));
            }

            estimate.set_x0(new_x0);
        }

        // Calculate the initial free energy estimate
        free_energies(0) = initial_free_energy_estimate(history, estimate.get_lnG(), sum_N, support_n, estimate.get_x0());

        if (restricted_individual_support) {
            // Set up the GMH equations and solve the to get a estimate of the free energy (c.f. section 4.1 in [JFB02])
            GMHequations eqn(history, sum_N, lnG_support, support_n, estimate.get_x0(), estimate.get_lnG()(estimate.get_x0()));

            int info = newton(free_energies, eqn);

            if (info!=0) {
                throw MLENoSolutionException();
            }

            // Calculate the entropy from the estimated free energies using equation (4.2) in [JFB02].
            DArray new_lnG(estimate.get_shape());
            calc_lnG(history, sum_N, lnG_support, support_n, free_energies, new_lnG);
            estimate.set_lnG(new_lnG);
        }
        else {
            // Calculate the accumulated support
            std::vector<CArray> accumulated_N(history.get_size());
            CArray sum_N(history.get_shape());

            for (MultiHistogramHistory::const_reverse_iterator set=history.rbegin(); set!=history.rend(); ++set) {
                for (unsigned int bin=0; bin < sum_N.get_asize(); ++bin) {
                    sum_N(bin) += (*set)->get_N()(bin);
                }
                accumulated_N[history.rend()-set-1] = sum_N;
            }

            // Set up the GMH equations and solve the to get a estimate of the free energy (c.f. section 4.1 in [JFB02])
            GMHequationsAccumulated eqn(history, sum_N, accumulated_N, lnG_support, support_n, estimate.get_x0(), estimate.get_lnG()(estimate.get_x0()));

            int info = newton(free_energies, eqn);

            if (info!=0) {
                throw MLENoSolutionException();
            }

            // Calculate the entropy from the estimated free energies using equation (4.2) in [JFB02].
            DArray new_lnG(estimate.get_shape());
            calc_lnG_accumulated(history, sum_N, accumulated_N, lnG_support, support_n, free_energies, new_lnG);
            estimate.set_lnG(new_lnG);
        }

        estimate.set_lnG_support(lnG_support);

        // Clear the old estimated free energies and set the new estimates
        estimate.free_energies.clear();
        estimate.free_energies_array = free_energies;

        for (unsigned int set=0; set<history.get_size(); set++) {
            estimate.free_energies[&history[set]] = free_energies(set);
        }

        // Print the estimated free energies
        MessageLogger::get().debug("MLE estimated free energies are: " + free_energies.write(3, false, false));

        // Calculate and print the minimal and maximal beta
        if (history.get_shape().size()==1) {
            // Find left and right bound of the support
            unsigned int bin_left = MLEutils::SupportBoundaries1D::find_left_bound(lnG_support);
            unsigned int bin_right = MLEutils::SupportBoundaries1D::find_right_bound(lnG_support);

            double beta_left = 0;
            double beta_right = 0;

            // Calculate lng
            if (binner != NULL) {
                DArray bin_centers = binner->get_binning_centered();
                DArray lng = estimate.get_lnG() - TArray_log<DArray, DArray>(binner->get_bin_widths());

                MLEutils::AverageSlope1d<DArray> average_slope(lng, lnG_support, sum_N, bin_centers, sigma);
                beta_left = average_slope.get_slope(bin_left);
                beta_right = average_slope.get_slope(bin_right);
            }
            else {
                MLEutils::AverageSlope1dUniform average_slope(estimate.get_lnG(), lnG_support, sum_N, sigma);
                beta_left = average_slope.get_slope(bin_left);
                beta_right = average_slope.get_slope(bin_right);
            }

            MessageLogger::get().debug("MLE beta range: (" + to_string(beta_left) + ", " + to_string(beta_right) + ")");
        }
    }
}

double MLE::initial_free_energy_estimate(const MultiHistogramHistory &history, const DArray &lnG, const CArray &sum_N, const CArray &support_n, const std::vector<Index> x0) {
    // TODO: This code assumes that only the first histogram is the new one. This should be generalized.

    if (history.get_size()==1) {
        // If there is only one histogram in the history, the free energy is estimated based on the reference entropy in x0.
        // See equation (2.19) in [JFB02] for details, c.f. point (1) on page 116.
        return -lnG(x0) - history[0].get_lnw()(x0) - log(support_n(0)) + log(history[0].get_N()(x0));
    }
    else {
        // Else the initial guess is defined as described in equation (A.4) in [JFB02].

        // Find the regions where we can use the old estimate of lnG.
        // If sum_N-history[0].get_N() >= minount then we know that lnG has been estimated for this been
        BArray usable = (sum_N - history[0].get_N()) >= min_count && history[0].get_N() > 0;

        // Calculate the total number of counts within the usable region (n_in) for the 0'th histogram
        unsigned int n_in = history[0].get_N().sum(usable);

        // Calculate the total number of counts outside the usable region (n_out) for the 0'th histogram.
        // Note that we use support_n as the total number of counts for the histogram, since we are not going to use those bins, we the is no support.
        unsigned int n_out = support_n(0) - n_in;

        // If n_in is 0 then there is no overlap with previous histograms
        if(n_in == 0) {
            throw MLENoOverlapException();
        }

        // First calculate the partition function within the usable area, as given by equation (A.5) in [JFB02].
        DArray summands(history.get_shape());

        for (BArray::constwheretrueiterator it = usable.get_constwheretrueiterator(); it(); ++it) {
            summands(it) = lnG(it) + history[0].get_lnw()(it);
        }

        double ln_z_in = log_sum_exp(summands, usable);

        // Calculate partition function as z = z_in(1+n_out/n_in) from equation (A.4) and further explained on page 117 in [JFB02].
        double lnz = ln_z_in + log(1.0 + static_cast<double> (n_out) / static_cast<double> (n_in));

        return -lnz;
    }
}

void MLE::extend_estimate(const History &extended_history, Estimate &estimate, const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
    estimate.extend(add_under, add_over);
}

void MLE::calc_lnG(const MultiHistogramHistory &history, const CArray &sum_N, const BArray &support, const CArray &support_n, const DArray &free_energy, DArray &new_lnG) {
    DArray summands(history.get_size());

    for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
        // Calculate the sum in the denominator of equation (4.2) in [JBF02] using "log sum exp"
        for (unsigned int i=0; i < history.get_size(); i++) {
            if (history[i].get_N()(it) > 0) {
                summands(i) = log(support_n(i)) + history[i].get_lnw()(it) + free_energy(i);
            }
            else {
                summands(i) = -std::numeric_limits<double>::infinity();
            }
        }

        double log_denominator = log_sum_exp(summands);

        // Calculate the entropy using equation (4.2) in [JBF02]
        new_lnG(it) = log(sum_N(it)) - log_denominator;
    }
}

void MLE::calc_lnG_accumulated(const MultiHistogramHistory &history, const CArray &sum_N, const std::vector<CArray> &accumulated_N, const BArray &support, const CArray &support_n, const DArray &free_energy, DArray &new_lnG) {
    DArray summands(history.get_size());

    for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
        // Calculate the sum in the denominator of equation (4.2) in [JBF02] using "log sum exp"
        for (unsigned int i=0; i < history.get_size(); i++) {
            if (history[i].get_N()(it) > 0) {
                summands(i) = log(support_n(i)) + history[i].get_lnw()(it) + free_energy(i);
            }
            else {
                summands(i) = -std::numeric_limits<double>::infinity();
            }
        }

        double log_denominator = log_sum_exp(summands);

        // Calculate the entropy using equation (4.2) in [JBF02]
        new_lnG(it) = log(sum_N(it)) - log_denominator;
    }
}

Estimate* MLE::new_estimate(const DArray &lnG, const BArray &lnG_support, const std::vector<unsigned int> &x0, const DArray &free_energies, const History &base_history, const Binner *binner) {
	assert(lnG.same_shape(lnG_support) && lnG.has_shape(base_history.get_shape()));

    // Cast the history and estimate to be the MLE type
    const MultiHistogramHistory& history = MultiHistogramHistory::cast_from_base(base_history, "The MLE estimator is only compatible with the MultiHistogramHistory.");
    assert(free_energies.get_ndims()==1 && free_energies.get_shape(0)==history.get_size());

	// Make a new MLE estimate
	MLEestimate *estimate = new MLEestimate(lnG, lnG_support, x0);

	// Calculate the support for the individual histogram in the history as the
	// intersection between lnG_support and the history
	std::vector<CArray> supports(history.get_size());

	// Set the free energy
	estimate->free_energies_array = free_energies;

	for (MultiHistogramHistory::const_iterator set=history.begin(); set!=history.end(); ++set) {
        unsigned int i = set - history.begin();
        estimate->free_energies[*set] = free_energies(i);
    }

    // Get an accurate estimate of the free energies (the estimate is set
    // automatically by the estimate function).
    this->estimate(base_history, *estimate, binner);

    return estimate;
}

} // namespace Muninn

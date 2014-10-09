// NonUniformDynamicBinner.h
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

#ifndef MUNINN_NONUNIFORMDYNAMICBINNER_H_
#define MUNINN_NONUNIFORMDYNAMICBINNER_H_

#include <algorithm>
#include <cmath>

#include "muninn/common.h"
#include "muninn/Binner.h"
#include "muninn/Binners/NonUniformBinner.h"
#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/Exceptions/MaximalNumberOfBinsExceed.h"

#include "muninn/utils/polation/SupportBoundaries.h"
#include "muninn/utils/polation/AverageSlope.h"
#include "muninn/utils/BaseConverter.h"

namespace Muninn {

/// This class implements an automatic non uniform binning procedure. The bins
/// are set so that a uniform resolution, \f$ r \f$, in the weights is
/// obtained, \f$ |\ln w(E_j) - \ln w(E_{j+1})| \simeq r \f$.
class NonUniformDynamicBinner : public NonUniformBinner, public BaseConverter<Binner, NonUniformDynamicBinner> {
public:
    /// Constructor
    ///
    /// \param resolution The resolution for the binner. The binning is set so
    ///                   that a uniform resolution, \f$ r \f$, in the weights
    ///                   is obtained, \f$ |ln w(E_j) - \ln w(E_{j+1})| \simeq r \f$.
    /// \param initial_width_is_max_left Use the initial bin with as maximal
    ///                                  bin width, when expanding to the left.
    /// \param initial_width_is_max_right Use the initial bin with as maximal
    ///                                   bin width, when expanding to the right.
    /// \param max_number_of_bins The maximal number of bins the binner can use.
    /// \param extend_factor When extending the binned area the extension is
    ///                      padded with extend_factor/resolution bins.
    /// \param sigma The number of observed bins used in the Gaussian kernel
    ///              for the slope estimate of the weights. The estimated is
    ///              used to obtained a constant resolution in the weights.
    NonUniformDynamicBinner(double resolution=0.2, bool initial_width_is_max_left=true, bool initial_width_is_max_right=false,
                            unsigned int max_number_of_bins=1000000, double extend_factor=1.0, unsigned int sigma = 20) :
        NonUniformBinner(), resolution(resolution), initial_width_is_max_left(initial_width_is_max_left),
        initial_width_is_max_right(initial_width_is_max_right), max_number_of_bins(max_number_of_bins),
        extend_factor(extend_factor), sigma(sigma), initial_bin_width(0.0),
        use_preset_slopes(false), preset_slope_left_bound(0.0), preset_slope_right_bound(0.0) {};

    /// Constructor based on a previous estimated binning.
    ///
    /// \param binning A previous estimated binning to start from.
    /// \param beta The value used of the initial beta, used for estimating the
    ///             binning.
    /// \param resolution The resolution for the binner. The binning is set so
    ///                   that a uniform resolution, \f$ r \f$, in the weights
    ///                   is obtained, \f$ |ln w(E_j) - \ln w(E_{j+1})| \simeq r \f$.
    /// \param initial_width_is_max_left Use the initial bin with as maximal
    ///                                  bin width, when expanding to the left.
    /// \param initial_width_is_max_right Use the initial bin with as maximal
    ///                                   bin width, when expanding to the right.
    /// \param max_number_of_bins The maximal number of bins the binner can use.
    /// \param extend_factor When extending the binned area the extension is
    ///                      padded with extend_factor/resolution bins.
    /// \param sigma The number of observed bins used in the Gaussian kernel
    ///              for the slope estimate of the weights. The estimated is
    ///              used to obtained a constant resolution in the weights.
    NonUniformDynamicBinner(const DArray& binning, double beta, double resolution=0.2, bool initial_width_is_max_left=true, bool initial_width_is_max_right=false,
                            unsigned int max_number_of_bins=1000000, double extend_factor=1.0, unsigned int sigma = 20) :
        NonUniformBinner(binning), resolution(resolution), initial_width_is_max_left(initial_width_is_max_left),
        initial_width_is_max_right(initial_width_is_max_right), max_number_of_bins(max_number_of_bins),
        extend_factor(extend_factor), sigma(sigma), initial_bin_width(0.0),
        use_preset_slopes(false), preset_slope_left_bound(0.0), preset_slope_right_bound(0.0) {
        if (std::abs(beta)<1E-6) {
            initial_bin_width = get_bin_widths().max();
        }
        else {
        	initial_bin_width = std::abs(resolution/beta);
        }
    };

    // Implementation of Binner interface (see base class for documentation).
    virtual void initialize(std::vector<double> &initial_values, double beta=0.0) {
        // If the initial sampling was done a beta=0, we will use the beta one std away
        // from the mean, as the initial beta
        if (std::abs(beta)<1E-6) {
            // Find the quantiles corresponding to +/- a standard deviation
            std::vector<double> fractiles = calculate_fractiles(initial_values, newvector(0.1586553, 0.8413447));

            // Check that the quantiles differ
            if (!(fractiles[1]-fractiles[0]>0)) {
                throw MessageException("An initial binning could not be estimate since the 16% and 84% fractiles for the sampled energies have the same value. This means that 68% of the sampled energies have the same value.");
            }

            // Find the standard deviation and the bin width
            double sigma = 0.5*(fractiles[1]-fractiles[0]);

            // Set the beta calculate one std away
            beta = 1.0/sigma;
        }
        else {
            // Else just sort the values (this is done implicit by calculate_fractiles)
            std::sort(initial_values.begin(), initial_values.end());
        }

        // Calculate the initial bin width
        initial_bin_width = std::abs(resolution/beta);

        // Find the min and max values among the reported values
        double min_value = initial_values[0] - initial_bin_width/2.0;
        double max_value = initial_values[initial_values.size() - 1] + initial_bin_width/2.0;

        // Check that the minimal and maximal value are finite
        if (!std::isfinite(min_value) || !std::isfinite(max_value)) {
            throw MessageException("An initial binning could not be estimate since a non finite energy has been added to Muninn.");
        }

        // Find the number of bins
        double nbins_double_precision = (max_value - min_value) / initial_bin_width + 1;
        nbins = static_cast<unsigned int>(nbins_double_precision);

        // Check that the maximal number of bins has not been exceeded
        if (nbins_double_precision > static_cast<double>(max_number_of_bins) ) {
            throw MaximalNumberOfBinsExceed(max_number_of_bins, nbins>0 ? nbins : std::numeric_limits<unsigned int>::max());
        }

        // Set the binning array
        binning = DArray(nbins+1);

        for (unsigned int i=0; i<nbins+1; ++i) {
            binning(i) = min_value + i*initial_bin_width;
        }

        // Print info and update initialized state
        MessageLogger::get().info("Setting initial bin width to: "+to_string(initial_bin_width));
        initialized = true;
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extend(double value, const Estimate &estimate, const History &base_history, const DArray &lnw) {
        int bin = calc_bin(value);

        // Cast the history to be a MultiHistogramHistory
        const MultiHistogramHistory& history = MultiHistogramHistory::cast_from_base(base_history, "The NonLinearDynamicBinner is only compatible with the MultiHistogramHistory.");

        // Check if we are outside the range and calculate how much to add
        std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension(newvector(0u), newvector(0u));

        if (bin < 0) {
            // Find the average slope on the left bound of the weights
            unsigned int bin_left = MLEutils::SupportBoundaries1D::find_left_bound(estimate.get_lnG_support());
            double slope = use_preset_slopes ? preset_slope_left_bound : MLEutils::AverageSlope1d<DArray>::get_slope(bin_left, lnw, estimate.get_lnG_support(), history.get_sum_N(), this->get_binning_centered(), sigma);

            // Calculate the new bin width
            double bin_width = std::abs(resolution/slope);

            // Possible, limit the the bin width by the initial bin width
            if (initial_width_is_max_left)
                bin_width = std::min(bin_width, initial_bin_width);

            // Find the number of bins to add
            unsigned int to_add_double_precision = (binning(0) - value)/bin_width + 1 + extend_factor/resolution;
            unsigned int to_add = static_cast<unsigned int>(to_add_double_precision);

            // Check that the maximal number of bins had not been exceeded (do it in double to avoid overflow
            if (to_add_double_precision + static_cast<double>(nbins) > static_cast<double>(max_number_of_bins)) {
                throw MaximalNumberOfBinsExceed(max_number_of_bins, nbins+to_add);
            }

            // Updated the number of bins and the binning array
            nbins += to_add;
            binning = binning.extended(to_add, 0u);
            for (unsigned int index=0; index<to_add; ++index) {
                binning(index) = binning(to_add) - (to_add-index)*bin_width;
            }

            // Updated the extension vector
            extension.first = newvector(to_add);

            // Print info
            MessageLogger::get().debug("Extending to "+to_string(nbins)+" bins with new width: "+to_string(bin_width));
        }
        else if (bin >= static_cast<int>(nbins) ) {
            // Find the average slope on the right bound of the weights
            unsigned int bin_right = MLEutils::SupportBoundaries1D::find_right_bound(estimate.get_lnG_support());
            double slope = use_preset_slopes ? preset_slope_right_bound : MLEutils::AverageSlope1d<DArray>::get_slope(bin_right, lnw, estimate.get_lnG_support(), history.get_sum_N(), this->get_binning_centered(), sigma);

            // Calculate the new bin width
            double bin_width = std::abs(resolution/slope);

            // Possible, limit the the bin width by the initial bin width
            if (initial_width_is_max_right)
                bin_width = std::min(bin_width, initial_bin_width);

            // Find the number of bins to add
            unsigned int prev_nbins = binning.get_shape(0)-1;
            double to_add_double_precision = (value-binning(prev_nbins))/bin_width + 1 + extend_factor/resolution;
            unsigned int to_add = static_cast<unsigned int>(to_add_double_precision);

            // Check that the maximal number of bins had not been exceeded (do it in double to avoid overflow
            if (to_add_double_precision + static_cast<double>(nbins) > static_cast<double>(max_number_of_bins)) {
                throw MaximalNumberOfBinsExceed(max_number_of_bins, nbins+to_add);
            }

            // Updated the number of bins and the binning array
            nbins += to_add;
            binning = binning.extended(0u, to_add);

            for (unsigned int index=prev_nbins+1; index<=nbins; ++index) {
                binning(index) = binning(prev_nbins) + (index-prev_nbins)*bin_width;
            }

            // Updated the extension vector
            extension.second = newvector(to_add);

            // Print info
            MessageLogger::get().debug("Extending to "+to_string(nbins)+" bins with new width: "+to_string(bin_width));
        }

        return extension;
    }

    ///  Function for extending the binned region to include a new energy value,
    ///  without adding additional bins for padding.
    ///
    /// \param value This energy value should now be included in the binned region.
    /// \param estimate The newest estimate of the density of states.
    /// \param history The current history.
    /// \param lnw The curren log weights
    /// \return The number of bins added in respectively lower and upper end of the histogram
    std::pair<std::vector<unsigned int>, std::vector<unsigned int> > include(double value, const Estimate &estimate, const History &base_history, const DArray &lnw) {
        double tmp_extend_factor = extend_factor;
        extend_factor = 0.0;
        std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension = extend(value, estimate, base_history, lnw);
        extend_factor = tmp_extend_factor;
        return extension;
    }

    /// Preset the slope that is used when extending the binning
    ///
    /// \param slope_left_bound The slope used for extending to the left
    /// \param slope_right_bound The slope used for extending to the right
    void set_slopes(double slope_left_bound, double slope_right_bound) {
        use_preset_slopes = true;
        preset_slope_left_bound = slope_left_bound;
        preset_slope_right_bound = slope_right_bound;
    }

    /// Reset the slope values and use the slope of the weights
    void reset_slopes() {
        use_preset_slopes = false;
    }

    /// Get the sigma value
    ///
    /// \return The sigma value.
    unsigned int get_sigma() {
        return sigma;
    }

private:
    const double resolution;               ///< The (constant) resolution used for binning.
    const bool initial_width_is_max_left;  ///< Use the initial bin with as maximal bin width, when expanding to the left.
    const bool initial_width_is_max_right; ///< Use the initial bin with as maximal bin width, when expanding to the right.
    const unsigned int max_number_of_bins; ///< The maximal number of bins the binner can use.
    double extend_factor;                  ///< When extending the binned area the extension is padded with extend_factor/resolution bins.
    const unsigned int sigma;              ///< The number of observed bins used in the Gaussian kernel for the slope estimate of the weights.
    double initial_bin_width;              ///< The bin width estimated based on the initial samples in the function initialize().

    bool use_preset_slopes;                ///< If true the, the binner will use the preset slopes for extending the binning
    double preset_slope_left_bound;        ///< Preset slope used when expanding to the left
    double preset_slope_right_bound;       ///< Preset slope used when expanding to the right

};

} // namespace Muninn

#endif /* MUNINN_NONUNIFORMDYNAMICBINNER_H_ */

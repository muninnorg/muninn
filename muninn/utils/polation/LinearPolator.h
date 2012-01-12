// LinearPolator.h
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

#ifndef MUNINN_LINEARPOLATOR_H_
#define MUNINN_LINEARPOLATOR_H_

#include <utility>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/polation/AverageSlope.h"
#include "muninn/utils/polation/SupportBoundaries.h"

namespace Muninn {
namespace MLEutils {

/// Class for doing linear interpolation and extrapolation on one-dimensional
/// functions, described by an array of function values and an array of bin
/// values.
///
/// \tparam BINNINGARRAY The type of the array representing the binning.
template <class BINNINGARRAY = DArray>
class LinearPolator1d : AverageSlope1d<BINNINGARRAY> {
public:

    /// Constructor.
    ///
    /// \param S The function values to do linear inter- and extrapolation of.
    /// \param support The support of function.
    /// \param N The counts for each value
    /// \param bin_centers The values of the center of the bins.
    /// \param min_obs_bins_per_std The minimal observations per standard deviations in the Gaussian kernel
    LinearPolator1d(DArray &S, const BArray &support, const CArray &N, const BINNINGARRAY &bin_centers, unsigned int min_obs_bins_per_std) :
        AverageSlope1d<BINNINGARRAY>(S, support, N, bin_centers, min_obs_bins_per_std), S(S) {};

    /// Default destructor
    virtual ~LinearPolator1d() {};

    /// Do linear extrapolation of the function values (S).
    ///
    /// \param slope_factor_up A factor to multiply the slope with, when the
    ///                        function is increasing in the direction away
    ///                        from the main area of support.
    /// \param slope_factor_down A factor to multiply the slope with, when the
    ///                          function is increasing in the direction away
    ///                          from the main area of support.
    /// \param min_slope_left The maximal allowed slope used for extrapolation
    ///                       on the left side of the support.
    /// \param max_slope_left The maximal allowed slope used for extrapolation
    ///                       on the left side of the support.
    /// \param min_slope_right The maximal allowed slope used for extrapolation
    ///                        on the right side of the support.
    /// \param max_slope_right The maximal allowed slope used for extrapolation
    ///                        on the right side of the support.
    /// \return A pair for the left bound (first) and the right (bound). Each
    ///         pair contains the bin index where the extrapolation starts (the
    ///         first bin with support) and the value of the slope used.
    std::pair<std::pair<unsigned int, double>, std::pair<unsigned int, double> >
    extrapolate(const double slope_factor_up, const double slope_factor_down,
            double min_slope_left=-std::numeric_limits<double>::infinity(),
            double max_slope_left= std::numeric_limits<double>::infinity(),
            double min_slope_right=-std::numeric_limits<double>::infinity(),
            double max_slope_right=std::numeric_limits<double>::infinity()) {
        std::pair<unsigned int, double> left = extrapolate_left(slope_factor_up, slope_factor_down, min_slope_left, max_slope_left);
        std::pair<unsigned int, double> right = extrapolate_right(slope_factor_up, slope_factor_down, min_slope_right, max_slope_right);

        return std::pair<std::pair<unsigned int, double>, std::pair<unsigned int, double> > (left, right);
    }

    /// Do linear extrapolation of the function values (S) on the left side of
    /// the support.
    ///
    /// \param slope_factor_up A factor to multiply the slope with, when the
    ///                        function is increasing in the direction away
    ///                        from the main area of support.
    /// \param slope_factor_down A factor to multiply the slope with, when the
    ///                          function is increasing in the direction away
    ///                          from the main area of support.
    /// \param min_slope The maximal allowed slope used for extrapolation
    ///                  on the left side of the support.
    /// \param max_slope The maximal allowed slope used for extrapolation
    ///                  on the left side of the support.
    /// \return A pair with the bin index where the extrapolation starts (the
    ///         first bin with support) and the value of the slope used.
    std::pair<unsigned int, double> extrapolate_left(const double slope_factor_up, const double slope_factor_down, double min_slope, double max_slope) {
        // Find the left side of the support
        unsigned int bin0 = SupportBoundaries1D::find_left_bound(this->support);

        // Find the average slope in bin_0
        double average_alpha = this->get_slope(bin0);

        // Calculate the slope to be used for the extrapolation based on whether the curve is moving up or down, when leaving the moving left
        double alpha = average_alpha>0 ? slope_factor_down*average_alpha : slope_factor_up*average_alpha;
        alpha = limit(alpha, min_slope, max_slope);

        // Do the extrapolation
        for (unsigned int bin=0; bin<bin0; ++bin)
            S(bin) = extrapolation_function(S, this->bin_centers, alpha, bin0, bin);

        return std::pair<unsigned int, double> (bin0, alpha);
    }

    /// Do linear extrapolation of the function values (S) on the right side of
    /// the support.
    ///
    /// \param slope_factor_up A factor to multiply the slope with, when the
    ///                        function is increasing in the direction away
    ///                        from the main area of support.
    /// \param slope_factor_down A factor to multiply the slope with, when the
    ///                          function is increasing in the direction away
    ///                          from the main area of support.
    /// \param min_slope The maximal allowed slope used for extrapolation
    ///                  on the right side of the support.
    /// \param max_slope The maximal allowed slope used for extrapolation
    ///                  on the right side of the support.
    /// \return A pair with the bin index where the extrapolation starts (the
    ///         last bin with support) and the value of the slope used.
    std::pair<unsigned int, double> extrapolate_right(const double slope_factor_up, const double slope_factor_down, double min_slope, double max_slope) {
        // Find the right side of the support
        unsigned int bin0 = SupportBoundaries1D::find_right_bound(this->support);

        // Find the average slope in bin_0
        double average_alpha = this->get_slope(bin0);

        // Calculate the slope to be used for the extrapolation based on whether the curve is moving up or down, when leaving the moving left
        double alpha = average_alpha>0 ? slope_factor_up*average_alpha : slope_factor_down*average_alpha;
        alpha = limit(alpha, min_slope, max_slope);

        // Do the extrapolation
        for (unsigned int bin=bin0+1; bin<S.get_shape(0); ++bin)
            S(bin) = extrapolation_function(S, this->bin_centers, alpha, bin0, bin);

        return std::pair<unsigned int, double> (bin0, alpha);
    }

    /// Do linear interpolation on the function values (S), in all internal
    /// regions where the function does not have support.
    void interpolate() {
        // Find the bin bounds of the internal parts without support
        std::vector<std::pair<unsigned int,unsigned int> > bounds;
        SupportBoundaries1D::find_internal_unsupported(this->support, bounds);

        // Do the interpolation
        for (std::vector<std::pair<unsigned int,unsigned int> >::iterator it = bounds.begin(); it != bounds.end(); ++it) {
            // Get the left and right bound
            unsigned int bin_left = it->first;
            unsigned int bin_right = it->second;

            // Calculate the slop for the interpolation.
            double slope = (S(bin_left)-S(bin_right)) / static_cast<double>(this->bin_centers(bin_left)-this->bin_centers(bin_right));

            // Interpolate the internal region.
            for (unsigned int bin = bin_left+1; bin < bin_right; ++bin) {
                S(bin) = interpolation_function(S, this->bin_centers, slope, bin_left, bin);
            }
        }
    }

    /// Calculate the value in of bin using an linear function for extrapolation.
    ///
    /// \param S The function values to do extrapolation on.
    /// \param binning An array representing the value in the bin centers.
    /// \param alpha The slope of the linear function.
    /// \param bin0 The bin index to start the linear extrapolation in.
    /// \param bin The bin index to evaluate the linear function in.
    /// \return The value of the linear function in the bin with index "bin".
    static inline double extrapolation_function(const DArray &S, const BINNINGARRAY &binning, double alpha, unsigned int bin0, unsigned int bin) {
        double x0 = binning(bin0);
        double x = binning(bin);
        return S(bin0) + alpha * (x - x0);
    }

    /// Calculate the value in of bin using an linear function for interpolation.
    ///
    /// \param S The function values to do interpolation on.
    /// \param binning An array representing the value in the bin centers.
    /// \param slope The slope of the linear function.
    /// \param bin_left The index of the bin to the left of the interval without support.
    /// \param bin The bin index to evaluate the linear function in.
    /// \return The value of the linear function in the bin with index "bin".
    static inline double interpolation_function(const DArray &S, const BINNINGARRAY &binning, const double slope, const unsigned int bin_left, const unsigned int bin) {
        double x_left = binning(bin_left);
        double x = binning(bin);
        return S(bin_left) + slope * (x - x_left);
    }

private:
    DArray &S;  ///< The function values to do linear inter- and extrapolation of (non-constant reference).
};

} // namespace Muninn
} // namespace MLEutils

#endif // LINEARPOLATOR_H_

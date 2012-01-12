// AverageSlope.h
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

#ifndef MUNINN_AVERAGESLOPE_H_
#define MUNINN_AVERAGESLOPE_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

namespace Muninn {
namespace MLEutils {

/// Class for calculating the average slope of a function using a Gaussian
/// kernel.
template <class BINNINGARRAY>
class AverageSlope1d {
public:
    /// Constructor for the slope calculator.
    ///
    /// \param S The function values to calculate the average slope of.
    /// \param support The support of the function.
    /// \param N The number of counts in each bin used to estimate the function values.
    /// \param bin_centers The values of the centers of the bins.
    /// \param min_obs_bins_per_std The minimal observations per standard deviations in the Gaussian kernel.
    AverageSlope1d(const DArray &S, const BArray &support, const CArray &N, const BINNINGARRAY &bin_centers, Count min_obs_bins_per_std) :
        S(S), support(support), N(N), bin_centers(bin_centers), min_obs_bins_per_std(min_obs_bins_per_std) {
        assert(S.get_ndims() == 1);
        assert(N.get_ndims() == 1);
    };

    /// Default destructor.
    virtual ~AverageSlope1d() {}

    /// Calculate the average slope of the function in a given bin.
    ///
    /// \param bin0 The bin to calculate the average slope in.
    /// \return The average slope in the bin.
    double get_slope(unsigned int bin0) {
        return get_slope(bin0, S, support, N, bin_centers, min_obs_bins_per_std);
    }

    /// Static function for calculating the average slope of the function in a given bin.
    ///
    /// \param bin0 The bin to calculate the average slope in.
    /// \param S The function values to calculate the average slope of.
    /// \param support The support of the function.
    /// \param N The number of counts in each bin used to estimate the function values.
    /// \param bin_centers The values of the centers of the bins.
    /// \param min_obs_bins_per_std The minimal observations per standard deviations in the Gaussian kernel.
    /// \return The average slope in the bin.
    static inline double get_slope(unsigned int bin0, const DArray &S, const BArray &support, const CArray &N, const BINNINGARRAY &bin_centers, Count min_obs_bins_per_std) {
        // Find the start and end to smooth over
        static const int std_windows = 3;              // The number std windows to include
        const double x0 = bin_centers(bin0);

        unsigned int start = bin0;
        unsigned int end = bin0;
        unsigned int nsupport = 0;

        while((nsupport<std_windows*min_obs_bins_per_std) && (start!=0 || end!=(S.get_shape(0)-1))) {
            double start_dist = 0<start ? x0-bin_centers(start-1) : std::numeric_limits<double>::infinity();
            double end_dist = end<(S.get_shape(0)-1) ? bin_centers(end+1)-x0 : std::numeric_limits<double>::infinity();

            // NOTE: This is not symmetric, if start_dist==end_dist then bins are first added on the left side
            if (start_dist < end_dist) {
                --start;
                nsupport += support(start) ? 1 : 0;
            }
            else {
                ++end;
                nsupport += support(end) ? 1 : 0;
            }
        }

        // Calculate the standard deviation of the normal distribution used to smoothing
        double sigma = std::max(x0-bin_centers(start), bin_centers(end)-x0)/static_cast<double>(std_windows);

        // Find the bins with support within the area [start, end]
        std::vector<unsigned int> support_bins;
        for(unsigned int bin=start; bin<=end; bin++)
            if (support(bin))
                support_bins.push_back(bin);

        // Calculate the sum in the enumerator and denominator.
        double enumerator = 0;
        double denumerator = 0;

        for (std::vector<unsigned int>::const_iterator  it=support_bins.begin(); it<support_bins.end()-1; it++) {
            double alpha = calc_alpha(S, bin_centers, *it, *(it+1));
            double w_poisson = calc_weight_poisson(N, *it, *(it+1));
            double w_sq = sq( bin_centers(*(it+1)) - bin_centers(*it) );
            double w_gaussian = calc_weight_gaussian(bin_centers, bin0, *it, *(it+1), sigma);

            enumerator += alpha * w_poisson * w_sq * w_gaussian;
            denumerator += w_poisson * w_sq * w_gaussian;
        }
        return enumerator/denumerator;
    }

protected:
    const DArray &S;                   ///< The function values to calculate the average slope on.
    const BArray &support;             ///< The support of the function.
    const CArray &N;                   ///< The number of counts in each bin used to estimate the function values.
    const BINNINGARRAY &bin_centers;   ///< The values of the centers of the bins.
    const Count min_obs_bins_per_std;  ///< The minimal observations per standard deviations in the Gaussian kernel.

private:
    /// Calculate the slope (alpha) based on two bins. The slope is defined  as
    /// \f[
    ///    \alpha(X_1) = ( S(X_2) - S(X_1) ) / (X_2-X_1)
    /// \f]
    /// where \f$ X_1 \f$ and \f$ X_2 \f$ are the value in the center of the two
    /// bins and \f$ S \f$ is the function.
    ///
    /// \param S The function values to calculate the slope of.
    /// \param bin_centers The values of the centers of the bins.
    /// \param bin1 The index of the first bin.
    /// \param bin2 The index of the second bin.
    /// \return The slope estimated from the two bins.
    ///
    /// TODO: Add references to the article.
    static inline double calc_alpha(const DArray &S, const BINNINGARRAY &bin_centers, unsigned int bin1, unsigned int bin2) {
        return (S(bin2)-S(bin1)) / static_cast<double>(bin_centers(bin2)-bin_centers(bin1));
    }

    /// Calculate the statistical Poisson weight for two bins, based on the number of counts in the sum
    /// histogram.
    ///
    /// \param N The number of counts the function S is estimated from (the sum histogram).
    /// \param bin1 The index of the first bin.
    /// \param bin2 The index of the second bin.
    /// \return The statistical Poisson weight.
    ///
    /// TODO: Added reference to the article.
    static inline double calc_weight_poisson(const CArray &N, unsigned int bin1, unsigned int bin2) {
        double denominator = N(bin2) + N(bin1);
        if (denominator > 0)
            return static_cast<double>(N(bin2)) * static_cast<double>(N(bin1)) / denominator;
        else
            return 0.0;
    }

    // Calculate the Gaussian weight by Gaussian kernel integration.
    ///
    /// \param bin_centers The values of the centers of the bins.
    /// \param bin0 The mean of the Gaussian.
    /// \param bin1 The index of the first bin.
    /// \param bin2 The index of the second bin.
    /// \param sigma The variance of the Gaussian.
    /// \return The Gaussian weight.
    ///
    /// TODO: Add reference to the article.
    static inline double calc_weight_gaussian(const BINNINGARRAY &bin_centers, unsigned int bin0, unsigned int bin1, unsigned int bin2, double sigma) {
        double x0 = bin_centers(bin0);
        double x = (bin_centers(bin1)+bin_centers(bin2))/2.0;
        return exp( -sq(x-x0) / (2*sq(sigma)) );
    }
};

} // namespace Muninn
} // namespace MLEutils

#endif // MUNINN_AVERAGESLOPE_H_

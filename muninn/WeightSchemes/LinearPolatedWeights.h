// LinearPolatedWeights.h
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

#ifndef LINEARPOLATEDWEIGHTS_H_
#define LINEARPOLATEDWEIGHTS_H_

#include <utility>
#include <limits>

#include "muninn/common.h"
#include "muninn/WeightScheme.h"
#include "muninn/ExtrapolatedWeightScheme.h"

namespace Muninn {

/// A class that modifies a weight scheme by doing linear interpolation and
/// extrapolation in the areas without support. This class is normally used
/// to wrap a normal weight scheme, and it only works for 1D simulations.
///
/// Extrapolation is done by calculating the slope in each of the two boundaries
/// of the area where the weights are defined (based on the support of lnG
/// according to the estimate). The slope is estimated as a weighted mean of
/// the slopes in an area around the boundary (using a Gaussian kernel). This
/// slope is then multiplied by a slope factor before it is used for
/// extrapolation.
///
/// Interpolation is done by connecting gaps in the weights by a linear function
/// such that the weight function becomes continuous.
class LinearPolatedWeigths : public ExtrapolatedWeightScheme {
public:

    /// The constructor for the linear polated wrapper weight scheme.
    ///
    ///
    /// \param underlying_weight_scheme The underlying weight scheme that is
    ///                                 being wrapped.
    /// \param slope_factor_up Slope factor used for the linear extrapolation
    ///                        of the weights, when the weights are increasing
    ///                        in the direction away from the main area of
    ///                        support.
    /// \param slope_factor_down Slope factor used for the linear extrapolation
    ///                          of the weights, when the weights are decreasing
    ///                          in the direction away from the main area of
    ///                          support.
    /// \param sigma The standard deviation (measured in number of bins with
    ///              support) of the Gaussian kernel used when calculating the
    ///              the weighted mean slope of the weights on the two outer
    ///              boundaries of the support. Also used for setting the size
    ///              of the margin when using min_beta_thermodynamics and
    ///              max_beta_thermodynamics
    /// \param min_beta_extrapolation The minimal allowed beta value (negative
    ///                               slope) for extrapolation. If the beta
    ///                               becomes smaller than this value it is
    ///                               capped at this value.
    /// \param max_beta_extrapolation The maximal allowed beta value (negative
    ///                               slope) for extrapolation. If the beta
    ///                               becomes larger than this value it is
    ///                               capped at this value.
    /// \param min_beta_thermodynamics The beta value is used to limit (modify)
    ///                                the slope of the weights within the area
    ///                                of support. If the mean energy at the
    ///                                given beta value falls inside the support
    ///                                of the weights (using a margin of size
    ///                                sigma), the slope of weights for energies
    ///                                higher than this mean is limited to be
    ///                                minimal min_beta_thermodynamics. In this
    ///                                case the slope in each bin is defined
    ///                                based on the two point estimate of the
    ///                                neighboring bin with higher energy.
    /// \param max_beta_thermodynamics The beta value is used to limit (modify)
    ///                                the slope of the weights within the area
    ///                                of support. If the mean energy at the
    ///                                given beta value falls inside the support
    ///                                of the weights (using a margin of size
    ///                                sigma), the slope of weights for energies
    ///                                lower than this mean is limited to be
    ///                                maximal max_beta_thermodynamics. In this
    ///                                case the slope in each bin is defined
    ///                                based on the two point estimate of the
    ///                                neighboring bin with lower energy.
    /// \param receives_ownership Whether the LinearPolatedWeigths class takes
    ///                           ownership of the underlying_weight_scheme.
    ///                           If set to true, the underlying_weight_scheme
    ///                           will be delete when the LinearPolatedWeigths
    ///                           object is deleted.
    LinearPolatedWeigths(WeightScheme *underlying_weight_scheme,
            double slope_factor_up=0.3,
            double slope_factor_down=3.00,
            unsigned int sigma=20,
            double min_beta_extrapolation=-std::numeric_limits<double>::infinity(),
            double max_beta_extrapolation= std::numeric_limits<double>::infinity(),
            double min_beta_thermodynamics=-std::numeric_limits<double>::infinity(),
            double max_beta_thermodynamics= std::numeric_limits<double>::infinity(),
            bool receives_ownership=false) :
                underlying_weight_scheme(underlying_weight_scheme),
                slope_factor_up(slope_factor_up), slope_factor_down(slope_factor_down), sigma(sigma),
                min_beta_extrapolation(min_beta_extrapolation), max_beta_extrapolation(max_beta_extrapolation),
                min_beta_thermodynamics(min_beta_thermodynamics), max_beta_thermodynamics(max_beta_thermodynamics),
                extrapolation_details(), left_bound_center(), right_bound_center(),
                has_ownership(receives_ownership) {}

    virtual ~LinearPolatedWeigths() {
        if (has_ownership) {
            delete underlying_weight_scheme;
        }
    }

    /// This method returns a set of weights. First the weights are calculated
    /// according to the underlying weight scheme, and afterwards interpolation
    /// and extrapolation is done according to the settings of the scheme.
    /// Interpolation and extrapolation is only done where there is no support
    /// (that is were estimate.get_lnG_support() is false).
    ///
    /// If min_beta_thermodynamics or max_beta_thermodynamics is specified, the
    /// weights may be modified inside the support.
    ///
    /// \param estimate The estimate of the entropy (lnG).
    /// \param base_history The history used for estimating the entropy.
    /// \param binner If the binning is not even, a binner should also be passed.
    /// \return Weights according to the weight scheme.
    virtual DArray get_weights(const Estimate &estimate, const History &base_history, const Binner *binner=NULL);

    // Implementation of ExtrapolatedWeightScheme interface (see base class for documentation).
    virtual double get_extrapolated_weight(double value, const DArray &lnw, const Estimate &estimate, const History &history, const Binner &binner);

    /// Set the minimal allowed beta value (negative slope) allowed for
    /// extrapolation. If the beta becomes smaller than this value it is
    /// capped at this value.
    ///
    /// \param min_beta The minimal allowed beta value for extrapolation.
    void set_min_beta_extrapolation(double min_beta=-std::numeric_limits<double>::infinity()) {
        min_beta_extrapolation = min_beta;
    }
    /// Set the maximal allowed beta value (negative slope) allowed for
    /// extrapolation. If the beta becomes larger than this value it is
    /// capped at this value.
    ///
    /// \param max_beta The maximal allowed beta value for extrapolation.
    void set_max_beta_extrapolation(double max_beta=std::numeric_limits<double>::infinity()) {
        max_beta_extrapolation = max_beta;
    }

    /// Set a minimal beta value used to limit (modify) the slope of the weights
    /// within the area of support. If the mean energy at the given beta value
    /// falls inside the support of the weights (using a margin of size sigma),
    /// the slope of weights for energies higher than this mean is limited to be
    ///  minimal min_beta_thermodynamics. In this case the slope in each bin is
    /// defined based on the two point estimate of the neighboring bin with
    /// higher energy.
    ///
    /// \param min_beta The minimal beta value to be used based on thermodynamics.
    void set_min_beta_thermodynamics(double min_beta=-std::numeric_limits<double>::infinity()) {
        min_beta_thermodynamics = min_beta;
    }

    /// Set a maximal beta value used to limit (modify) the slope of the weights
    /// within the area of support. If the mean energy at the given beta value
    /// falls inside the support of the weights (using a margin of size sigma),
    /// the slope of weights for energies lower than this mean is limited to be
    /// maximal max_beta_thermodynamics. In this case the slope in each bin is
    /// defined based on the two point estimate of the neighboring bin with
    /// higher energy.
    ///
    /// \param max_beta The minimal beta value to be used based on thermodynamics.
    void set_max_beta_thermodynamics(double max_beta=std::numeric_limits<double>::infinity()) {
        max_beta_thermodynamics = max_beta;
    }

private:
    WeightScheme *underlying_weight_scheme;  ///< The underlying weight scheme used for setting the weights on the support.

    double slope_factor_up;                  ///< Slope factor used for the linear extrapolation of the weights, when the weights are increasing in the direction away from the main area of support.
    double slope_factor_down;                ///< Slope factor used for the linear extrapolation of the weights, when the weights are decreasing in the direction away from the main area of support.
    unsigned int sigma;                      ///< The standard deviation (measured in number of bins with support) of the Gaussian kernel. Also used for setting the size of the margin, when limiting the beta thermodynamically.
    double min_beta_extrapolation;           ///< The minimal allowed beta value (negative slope) for extrapolation.
    double max_beta_extrapolation;           ///< The maximal allowed beta value (negative slope) for extrapolation.
    double min_beta_thermodynamics;          ///< A minimal beta value used to limit (modify) the slope of the weights within the area of support.
    double max_beta_thermodynamics;          ///< A maximal beta value used to limit (modify) the slope of the weights within the area of support.

    std::pair<std::pair<unsigned int, double>, std::pair<unsigned int, double> > extrapolation_details;  ///< The last used extrapolation details returned by MLEutils::LinearPolator1d::extrapolate.
    double left_bound_center;                ///< The center value for the left bound bin in the last extrapolation details (the center value of the bin extrapolation_details.first.first)
    double right_bound_center;               ///< The center value for the left bound bin in the last extrapolation details (the center value of the bin extrapolation_details.second.first)

    bool has_ownership;                      ///< Whether this object owns the underlying_weight_scheme object.

    /// Determines if at least min_support bins with lower bin index than bin0
    /// has support. It is assumed that bin0 < support.get_shape(0).
    ///
    /// \param min_support The required number of bins to have support.
    /// \param bin0 Check bins that has lower index than bin0
    /// \param support The support array.
    /// \return Returns true if at least min_support bins with index lower
    ///         than bin0 has support.
    static bool has_min_support_left(unsigned int min_support, unsigned int bin0, const BArray& support);

    /// Determines if at least min_support bins with higher bin index than bin0
    /// has support. It is assumed that bin0 < support.get_shape(0).
    ///
    /// \param min_support The required number of bins to have support.
    /// \param bin0 Check bins that has higher index than bin0.
    /// \param support The support array.
    /// \return Returns true if at least min_support bins with index higher
    ///         than bin0 has support.
    static bool has_min_support_right(unsigned int min_support, unsigned int bin0, const BArray& support);
};

} // namespace Muninn
#endif /* LINEARPOLATEDWEIGHTS_H_ */

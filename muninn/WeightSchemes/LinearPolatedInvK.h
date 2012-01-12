// LinearPolatedInvK.h
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

#ifndef MUNINN_LINEARPOLATEDINVK_H_
#define MUNINN_LINEARPOLATEDINVK_H_

#include "muninn/WeightScheme.h"
#include "muninn/WeightSchemes/InvK.h"
#include "muninn/WeightSchemes/LinearPolatedWeights.h"

namespace Muninn {

/// A 1/k weight scheme with linear interpolation and extrapolation. It is
/// simple shortcut to the InvK class wrapped by the LinearPolatedWeigths class.
class LinearPolatedInvK : public LinearPolatedWeigths {
public:

    /// Simple constructor for the LinearPolatedInvK weights. The values for
    /// min and max beta (for both extrapolation and thermodynamics) defaulted
    /// to minus infinity and plus infinity respectively. The values can be
    /// changed through the setters inherent from the LinearPolatedWeigths class.
    ///
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
    LinearPolatedInvK(double slope_factor_up=0.3, double slope_factor_down=3.0, unsigned int sigma=20) :
        LinearPolatedWeigths(this_weight_scheme, slope_factor_up, slope_factor_down, sigma) {}
private:
    InvK this_weight_scheme; ///< The underlying InvK weight scheme.
};

} // namespace Muninn

#endif /* MUNINN_LINEARPOLATEDINVK_H_ */

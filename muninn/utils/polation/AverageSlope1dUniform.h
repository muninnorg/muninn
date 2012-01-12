// AverageSlope1dUniform.h
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

#ifndef MUNINN_AVERAGE_SLOPE1D_UNIFORM_H_
#define MUNINN_AVERAGE_SLOPE1D_UNIFORM_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/polation/Identity.h"

namespace Muninn {
namespace MLEutils {

/// Class for calculating the average slope of a function using a Gaussian
/// kernel on uniform binning.
class AverageSlope1dUniform : public AverageSlope1d<Identity> {
public:
    /// Constructor
    ///
    /// \param S The function values to do linear inter- and extrapolation of.
    /// \param support The support of function.
    /// \param N The counts for each value
    /// \param min_obs_bins_per_std The minimal observations per standard deviations in the Gaussian kernel
    AverageSlope1dUniform(const DArray &S, const BArray &support, const CArray &N, Count min_obs_bins_per_std) :
        AverageSlope1d<Identity>(S, support, N, identity, min_obs_bins_per_std) {}

    /// Default destructor.
    virtual ~AverageSlope1dUniform() {}

private:
    Identity identity;  //< A identity operator used for representing uniform binning
};

} // namespace Muninn
} // namespace MLEutils

#endif // MUNINN_AVERAGE_SLOPE1D_UNIFORM_H_

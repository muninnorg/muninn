// LinearPolator1dUniform.h
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

#ifndef MUNINN_LINEAR_POLATOR1D_UNIFORM_H_
#define MUNINN_LINEAR_POLATOR1D_UNIFORM_H_

#include <utility>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/polation/LinearPolator.h"
#include "muninn/utils/polation/AverageSlope1dUniform.h"
#include "muninn/utils/polation/Identity.h"

namespace Muninn {
namespace MLEutils {

/// Class for doing linear interpolation and extrapolation on uniform binning
class LinearPolator1dUniform : public LinearPolator1d<Identity> {
public:
    /// Constructor
    ///
    /// \param S The function values to calculate the average slope on
    /// \param support The support of the values
    /// \param N The counts for each value
    /// \param min_obs_bins_per_std then minimal observations per standard deviations in the Gaussian kernel
    LinearPolator1dUniform(DArray &S, const BArray &support, const CArray &N, unsigned int min_obs_bins_per_std) :
        LinearPolator1d<Identity>(S, support, N, identity, min_obs_bins_per_std) {}

    virtual ~LinearPolator1dUniform() {}

private:
    // The default identity operator for uniform binning
    Identity identity;
};

} // namespace Muninn
} // namespace MLEutils

#endif // MUNINN_LINEAR_POLATOR1D_UNIFORM_H_

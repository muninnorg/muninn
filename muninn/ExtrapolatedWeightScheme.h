// ExtrapolatedWeightScheme.h
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

#ifndef MUNINN_EXTRAPOLATEDWEIGHTSCHEME_H_
#define MUNINN_EXTRAPOLATEDWEIGHTSCHEME_H_

#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/WeightScheme.h"
#include "muninn/Estimate.h"
#include "muninn/History.h"
#include "muninn/Binner.h"

namespace Muninn {

/// Interface for a weight schemes that support calculations of weights in bins
/// outside the current number of bins, without extending the number of bins.
/// The requires that a Binner is used.
class ExtrapolatedWeightScheme : public WeightScheme {
public:

    /// Default virtual destructor.
    virtual ~ExtrapolatedWeightScheme() {};

    /// Get the weight of an value falling falling outside the binned area,
    /// without extending the number of bins.
    ///
    /// \param value The value to get a weight for.
    /// \param lnw The current weights.
    /// \param estimate The current estimate of the entropy (lnG).
    /// \param history The history used for estimating the entropy.
    /// \param binner The binner describing the current binning.
    /// \return The weight corresponding to the given value.
    virtual double get_extrapolated_weight(double value, const DArray &lnw, const Estimate &estimate, const History &history, const Binner &binner) = 0;
};

} // namespace Muninn

#endif /* MUNINN_EXTRAPOLATEDWEIGHTSCHEME_H_ */

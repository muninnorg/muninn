// WeightScheme.h
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

#ifndef MUNINN_WEIGHTSCHEME_H_
#define MUNINN_WEIGHTSCHEME_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimate.h"
#include "muninn/History.h"
#include "muninn/Binner.h"

namespace Muninn {

/// The WeightScheme abstract class is the interface for classes implementing
/// weight schemes. The WeightScheme calculated weights based on a given
/// estimate (of the entropy) and possibly the history. If the binning is uneven,
/// a binner should also be passed.
class WeightScheme {
public:
    /// Default virtual destructor.
    virtual ~WeightScheme() {};

    /// This method returns a set of weights according to the implemented weight
    /// scheme, given an estimate, a history and possibly a binner.
    ///
    /// \param estimate The estimate of the entropy (lnG).
    /// \param history The history used for estimating the entropy.
    /// \param binner If the binning is not even, a binner should also be passed.
    /// \return Weights according to the weight scheme.
    virtual DArray get_weights(const Estimate &estimate, const History &history, const Binner *binner=NULL) = 0;
};

} // namespace Muninn

#endif /* MUNINN_WEIGHTSCHEME_H_ */

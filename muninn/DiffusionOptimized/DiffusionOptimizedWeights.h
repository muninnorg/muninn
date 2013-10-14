// Multicanonical.h
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

#ifndef MUNINN_DIFFUSION_OPTIMIZED_WEIGHTS_H_
#define MUNINN_DIFFUSION_OPTIMIZED_WEIGHTS_H_

#include "muninn/WeightScheme.h"
#include "muninn/DiffusionOptimized/DiffusionEstimate.h"
#include "muninn/utils/TArrayMath.h"

namespace Muninn {

/// The diffusion optimized weight scheme.
class DiffusionOptimizedWeights : public WeightScheme {
public:

    // Implementation of WeightScheme interface (see base class for documentation).
    virtual DArray get_weights(const Estimate &base_estimate, const History &history, const Binner *binner=NULL) {
        // To P&W: For now I have just implemented multicanoncial weights.
        DArray lnw(base_estimate.get_shape());

        for (BArray::constwheretrueiterator it = base_estimate.get_lnG_support().get_constwheretrueiterator(); it(); ++it) {
            lnw(it) = - base_estimate.get_lnG()(it);
        }

        if (binner && !binner->is_uniform()) {
            lnw += TArray_log<DArray, DArray>(binner->get_bin_widths());
        }

        // To P&W You should of cause return the weight based on your estimate
        DiffusionEstimate estimate = DiffusionEstimate::cast_from_base(base_estimate, "The DiffusionOptimizedWeights are only compatible with a DiffusionEstimate.");

        return lnw;
    }
};

} // namespace Muninn

#endif /* MUNINN_DIFFUSION_OPTIMIZED_WEIGHTS_H_ */

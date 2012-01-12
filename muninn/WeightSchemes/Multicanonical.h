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

#ifndef MUNINN_MULTICANONICAL_H_
#define MUNINN_MULTICANONICAL_H_

#include "muninn/WeightScheme.h"
#include "muninn/utils/TArrayMath.h"

namespace Muninn {

/// The multicanonical weight scheme, which works in any dimension. The weights
/// are set to the multicanoncial weights \f$ w(E) = 1/g(E) \f$ inside the
/// support, and outside the support the weights are set to 0. In the expression
/// above the function \f$ g\f$ is the density of states. Only an estimate of the
/// entropy \f$ ln(G) \f$ is available, so the density of states is approximated
/// by \f$ g(E) \approx G(E)/\Delta E \f$, where \f$ \Delta E \f$ is the bin
/// width.
///
/// In log-space this becomes \f$ \ln(w(E)) \approx \ln(\Delta E) - \ln(G(E)) \f$.
class Multicanonical : public WeightScheme {
public:

    // Implementation of WeightScheme interface (see base class for documentation).
    virtual DArray get_weights(const Estimate &estimate, const History &history, const Binner *binner=NULL) {
        // The multicanonical weight is set to w(E) = 1/g(E) = Delta(E)/G(E) or
        // in log space ln(w(E)) = -ln(g(E)) = ln(Delta(E) - ln(G(E)).
        DArray lnw(estimate.get_shape());

        for (BArray::constwheretrueiterator it = estimate.get_lnG_support().get_constwheretrueiterator(); it(); ++it) {
            lnw(it) = - estimate.get_lnG()(it);
        }

        if (binner && !binner->is_uniform()) {
            lnw += TArray_log<DArray, DArray>(binner->get_bin_widths());
        }

        return lnw;
    }
};

} // namespace Muninn

#endif /* MUNINN_MULTICANONICAL_H_ */

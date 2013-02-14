// InvKP.h
// Copyright (c) 2010-2012 Jes Frellsen, Jan Valentin
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

#ifndef MUNINN_INVKP_H_
#define MUNINN_INVKP_H_

#include <vector>

#include "muninn/WeightScheme.h"
#include "muninn/utils/TArrayUtils.h"

namespace Muninn {

/// The \f$ 1/k^p \f$ weight scheme for 1D simulations. The weights are set to the
/// \f$ 1/(k^p g^(1-p)) \f$ weights \f$ k(E) = (\sum_{E'<=E} G(E')) \f$
/// and outside the support the weights are set to 0; where \f$ E \f$ is the
/// statically weight  .
class InvKP : public WeightScheme {
private:
    double p;
public:
    
    InvKP(double p) {
        this->p = p;
    }
    // Implementation of WeightScheme interface (see base class for documentation).
    virtual DArray get_weights(const Estimate &estimate, const History &history, const Binner *binner=NULL) {

        // Check that the estimate is one dimensional
        assert(estimate.get_lnG().get_ndims() == 1);

        // Get the entropy and the support for the entropy
        const DArray& lnG = estimate.get_lnG();
        const BArray& lnG_support = estimate.get_lnG_support();

        // Make an new array for the g-weights
        DArray lnw_g(lnG.get_shape());

        for (BArray::constwheretrueiterator it = lnG_support.get_constwheretrueiterator(); it(); ++it) {
            lnw_g(it) = - lnG(it);
        }

        if (binner && !binner->is_uniform()) {
            lnw_g += TArray_log<DArray, DArray>(binner->get_bin_widths());
        }

        // Make an new array for the k-weights
        DArray lnw_k(lnG.get_shape());

        // Set the k-weights by summing up the DoS in the following way. Define
        // the log to k as:
        //    lnk_{i-1} = ln( sum_{E'<=E_{i-1}} g(E') )
        // Then
        //    lnk_i = ln( exp(lnk_{u-1}) + exp(lnG_i) )
        //          = ref + ln( exp(lnk_{i-1}-ref) + exp(lnG_i-ref) )
        // If we set ref=max(lnk_{i-1}, lnG_i) then we have for lnk_{i-1} > lnG_i
        //    lnk_i = lnk_{u-1} + ln( 1 + exp(lnG_i-lnk_{i-1}) )
        // and else
        //    lnk_i = lnk_{u-1} + ln( exp(lnk_{i-1} - lnG_i) )

        // Make an itterator over the support
        BArray::constwheretrueiterator it = lnG_support.get_constwheretrueiterator();

        // First check if there is any support
        if(it()) {
            // Calculate lnk_0 and set the weight
            double lnk = lnG(it);
            lnw_k(it) = -lnk;

            for (++it; it(); ++it) {
                // Calculate lnk_i
                if(lnG(it) > lnk)
                    lnk = lnG(it) + log( exp(lnk-lnG(it)) + 1 );
                else
                    lnk = lnk + log( 1 + exp(lnG(it)-lnk));

                // Set the weight and add the lnw_g log-weights
                lnw_k(it) = - this->p * lnk + (1 - this->p) * lnw_g(it);
            }

            // Shift the weights so that the bin with the maximal entropy has the
            // weight negative maximal entropy
            std::vector<unsigned int> max_index = arg_max(lnG, lnG_support);
            lnw_k += -lnG(max_index) -lnw_k(max_index);
        }

        return lnw_k;
    }
};

} // namespace Muninn

#endif /* MUNINN_INVKP_H_ */

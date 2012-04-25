// FixedWeights.h
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

#ifndef MUNINN_FIXEDWEIGHTS_H_
#define MUNINN_FIXEDWEIGHTS_H_

#include "muninn/common.h"
#include "muninn/WeightScheme.h"

namespace Muninn {

/// A class that modifies a weight scheme so that the weights become fixed in a
/// given region. This class should be used to wrap another weight scheme, and
/// it only works for 1D simulations.
class FixedWeights : public WeightScheme {
public:

    /// The constructor for the fixed weights wrapper weight scheme.
    ///
    /// \param reference_value The left most bin of the fixed weighs, will be
    ///                        the bin that this value fall in.
    /// \param fixed_weights The value of the fixed weights.
    /// \param underlying_weight_scheme The underlying weight scheme that is
    ///                                 being wrapped (this class takes
    ///                                 ownership of the object).
    FixedWeights(double reference_value,
                 const DArray& fixed_weights,
                 WeightScheme* underlying_weight_scheme) :
                     reference_value(reference_value),
                     fixed_weights(fixed_weights),
                     underlying_weight_scheme(underlying_weight_scheme) {
        assert(fixed_weights.nonempty());
    }

    /// Virtual destructor.
    virtual ~FixedWeights() {
        delete underlying_weight_scheme;
    };

    /// This method returns a set of weights. First the fixed_weights are calculated
    /// according to the underlying weight scheme, and afterwards the weights
    /// are changed in the fixed region, so that weight function is continuous
    //  at the intersection points.
    ///
    /// \param estimate The estimate of the entropy (lnG).
    /// \param base_history The history used for estimating the entropy.
    /// \param binner If the binning is not even, a binner should also be passed.
    /// \return Weights according to the weight scheme.
    virtual DArray get_weights(const Estimate &estimate, const History &base_history, const Binner *binner=NULL) {
        assert(binner!=NULL);

        // Get the weights from the underlying weight scheme
        DArray underlying_weights = underlying_weight_scheme->get_weights(estimate, base_history, binner);
        DArray weights(underlying_weights.get_shape());

        // Find the index of the reference bin
        int reference_index = binner->calc_bin(reference_value);

        // Copy the weights that has support and adjust the weight in the remaining regions.
        if (0<=reference_index && reference_index<static_cast<int>(weights.get_shape(0))) {

            // Calculate the maximal right most index
            int end_index = std::min(weights.get_shape(0), reference_index + fixed_weights.get_shape(0));

            // Adjust the weights left of the reference index
            for (int index=0; index<reference_index; ++index) {
                weights(index) = underlying_weights(index) - underlying_weights(reference_index) + fixed_weights(0);
            }

            // Copy the fixed weights
            for (int index=reference_index; index<end_index; ++index) {
                weights(index) = fixed_weights(index - reference_index);
            }

            // Adjust the weights rigt of the reference index
            for (int index=end_index; index<static_cast<int>(underlying_weights.get_shape(0)); ++index) {
                weights(index) = underlying_weights(index) - underlying_weights(end_index - 1) +  fixed_weights(fixed_weights.get_shape(0) - 1);
            }
        }

        return weights;
    }


private:
    double reference_value;                  ///< The reference value for the left most bin of the fixed weights.
    const DArray fixed_weights;              ///< The value of the fixed fixed_weights.
    WeightScheme *underlying_weight_scheme;  ///< The underlying weight scheme used for setting the weights on the support.
};

} // namespace Muninn

#endif // MUNINN_FIXEDWEIGHTS_H_

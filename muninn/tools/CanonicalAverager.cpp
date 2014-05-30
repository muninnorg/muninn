// CanonicalAverager.cpp
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

#include "muninn/tools/CanonicalAverager.h"

#include "muninn/utils/TArray.h"
#include "muninn/Binner.h"
#include "muninn/Binners/NonUniformBinner.h"

namespace Muninn {

std::vector<double> CanonicalAverager::calculate_weights(const std::vector<double> &energies, const Binner &binner, const DArray &lnG, const BArray &lnG_support, double beta) {
    // Assert the arrays got same shape and are one dimensional
    assert(lnG.same_shape(lnG_support));
    assert(lnG.get_shape().size()==1);

    unsigned int nbins = lnG.get_shape(0);

    // First make a histogram of the energies given as argument
    UArray histogram(nbins);

    for(std::vector<double>::const_iterator it=energies.begin(); it < energies.end(); it++) {
        int bin = binner.calc_bin(*it);

        if (0 <= bin && bin < static_cast<int>(nbins)) {
            histogram(bin)++;
        }
    }

    // Calculate the support for this histogram
    BArray histogram_support = histogram > 0;

    // Calculate the normalization constant for the canonical distribution,
    // by summing over the area with both the histogram and the estimated
    // entropy (lnG) has support. The normalization constant is given by
    //
    // lnZ_beta = log(Z_beta) = log[ sum_E exp( S(E) - beta*E ) ]
    //
    // To calculate this we use the log-sum-exp trick
    BArray support = histogram_support && lnG_support;
    DArray binning = binner.get_binning_centered();

    DArray summands(nbins);

    for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
        summands(it) = lnG(it) - beta*binning(it);
    }

    double lnZ_beta = log_sum_exp(summands, support);

    // Now calculate the probability for each bin according to the canonical ensemble
    DArray P_beta(nbins);

    for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
        P_beta(it) = exp(-beta*binning(it) + lnG(it) - lnZ_beta);
    }

    // Calculate the weight for each energy in the energy vector
    std::vector<double> weights;

    for(std::vector<double>::const_iterator it=energies.begin(); it < energies.end(); it++) {
        int bin = binner.calc_bin(*it);

        if (0 <= bin && bin < static_cast<int>(nbins)) {
            double weight = 1.0/static_cast<double>(histogram(bin)) * P_beta(bin);
            weights.push_back(weight);
        }
        else {
            weights.push_back(0);
        }
    }

    return weights;
}

std::vector<double> CanonicalAverager::calculate_weights(const std::vector<double> &energies, const CGE &cge, double beta) {
    // Assume that the CGE only has one dimension (which is the only thing implemented at the moment)
    assert(cge.get_ge().get_current_sum_histogram().get_shape().size() == 1);
    return calculate_weights(energies, cge.get_binner(), cge.get_ge().get_estimate().get_lnG(), cge.get_ge().get_estimate().get_lnG_support(), beta);
}

std::vector<double> CanonicalAverager::calculate_weights(const std::vector<double> &energies, const DArray &binning, const DArray &lnG, const BArray &lnG_support, double beta) {
    NonUniformBinner binner(binning);
    return calculate_weights(energies, binner, lnG, lnG_support, beta);
}

} // namespace Muninn

// CanonicalAverager.h
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

#ifndef MUNINN_CANONICALAVERAGER_H_
#define MUNINN_CANONICALAVERAGER_H_

#include <vector>
#include <string>

#include "muninn/CGE.h"
#include "muninn/Binners/NonUniformBinner.h"

namespace Muninn {

/// Class for calculating the weights for a canonical average over a set of
/// energies, given an estimate of the entropy (lnG) and a given value of beta.
///
/// Assume that we are given a sequence of observations \f$ (x_i) \f$, where
/// the \f$ i \f$'th observation has energy \f$ e_i \f$. Suppose we want to
/// calculate the the average of some function \f$ f(x) \f$ using according
/// to a canonical distribution with a given \f$ \beta \f$, that is
/// \f[
///     \langle f \rangle_\beta = \sum_i f(x_i) P_\beta(x_i)
/// \f]
/// where \f$ P_\beta(x_i) \f$ is the probability of the \f$ i \f$'th
/// observation according to the canonical distribution. This distribution can
/// be approximated by
/// \f[
///     P_\beta(x_i) = P_\beta(E_i) / n(E_i)
/// \f]
/// where \f$ E_i \f$ is the energy bin that the \f$ e_i \f$ falls in,
/// \f$ P_\beta(E_i) \f$ is the probability of the bin according to the
/// the canonical distribution and \f$ n(E_i) \f$ is the number of
/// observations falling in energy bin \f$ E_i \f$.
///
/// For a sequence of energies \f$ P_\beta(x_i) \f$ this functions in this
/// class can return the corresponding weights \f$ P_\beta(x_i) \f$.
class CanonicalAverager {
public:

    /// Constructor based on a binning array.
    ///
    /// \param binning An array containing the edges of the bins.
    /// \param lnG An estimate of the entropy.
    /// \param lnG_support The support of the entropy estimate.
    CanonicalAverager(const DArray &binning, const DArray &lnG, const BArray &lnG_support) :
        binner(NULL), lnG(&lnG), lnG_support(&lnG_support), has_binner_ownership(false) {
        set_binner_from_array(binning);
    }

    /// Constructor based on a binning object.
    ///
    /// \param binner A Binner that represents the binning of the entropy.
    /// \param lnG An estimate of the entropy.
    /// \param lnG_support The support of the entropy estimate.
    CanonicalAverager(const Binner &binner, const DArray &lnG, const BArray &lnG_support) :
        binner(&binner), lnG(&lnG), lnG_support(&lnG_support), has_binner_ownership(false) {}

    /// Default destructor.
    virtual ~CanonicalAverager() {
        if (has_binner_ownership) {
            delete binner;
        }
    }

    /// Function for calculating the weights corresponding to a sequence of
    /// energies.
    ///
    /// \param energies The sequence of energies to calculate weights for.
    /// \param beta The beta for the canonical distribution.
    /// \return The sequence of weights corresponding to the sequence of
    ///         energies.
    std::vector<double> calc_weights(const std::vector<double> &energies, double beta=1.0) const {
        std::vector<double> weights = calculate_weights(energies, *binner, *lnG, *lnG_support, beta);
        return weights;
    }

    /// Static function for calculating the the weights corresponding to a
    /// sequence of energies.
    ///
    /// \param energies The sequence of energies to calculate weights for.
    /// \param binner A Binner that represents the binning of the entropy.
    /// \param lnG An estimate of the entropy.
    /// \param lnG_support The support of the entropy estimate.
    /// \param beta The beta for the canonical distribution.
    /// \return The sequence of weights corresponding to the sequence of
    ///         energies.
    static std::vector<double> calculate_weights(const std::vector<double> &energies, const Binner &binner, const DArray &lnG, const BArray &lnG_support, double beta=1.0);

    /// Static function for calculating the the weights corresponding to a
    /// sequence of energies, based on the current estimate in a CGE
    /// object.
    ///
    /// \param energies The sequence of energies to calculate weights for.
    /// \param cge The cge object to obtain the estimate of the entropy (lnG) from
    /// \param beta The beta for the canonical distribution.
    /// \return The sequence of weights corresponding to the sequence of
    ///         energies.
    static std::vector<double> calculate_weights(const std::vector<double> &energies, const CGE &cge, double beta=1.0);

    /// Static function for calculating the the weights corresponding to a
    /// sequence of energies.
    ///
    /// \param energies The sequence of energies to calculate weights for.
    /// \param binning An array containing the edges of the bins.
    /// \param lnG An estimate of the entropy.
    /// \param lnG_support The support of the entropy estimate.
    /// \param beta The beta for the canonical distribution.
    /// \return The sequence of weights corresponding to the sequence of
    ///         energies.
    static std::vector<double> calculate_weights(const std::vector<double> &energies, const DArray &binning, const DArray &lnG, const BArray &lnG_support, double beta=1.0);

protected:
    const Binner *binner;       ///< The Binner that represents the binning of the entropy.
    const DArray *lnG;          ///< An estimate of the entropy.
    const BArray *lnG_support;  ///< The support of the entropy estimate.
    bool has_binner_ownership;  ///< If this variable is true, the object owns the binner object and deletes it whin this object is deleted.

    // Default constructor.
    CanonicalAverager() : binner(NULL), lnG(NULL), lnG_support(NULL), has_binner_ownership(false) {};

    /// Function for setting up the binner based on a array containing the
    /// edge values of the bins.
    ///
    /// \param binning An array containing the bin enges.
    void set_binner_from_array(const DArray &binning) {
        binner = new NonUniformBinner(binning);
        has_binner_ownership = true;
    }
};

} // namespace Muninn

#endif /* MUNINN_CANONICALAVERAGER_H_ */

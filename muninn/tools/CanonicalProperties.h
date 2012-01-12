// CanonicalProperties.h
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

#ifndef MUNINN_CANONICAL_PROPERTIES_H_
#define MUNINN_CANONICAL_PROPERTIES_H_

#include <cmath>

#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayMath.h"
#include "muninn/utils/TArrayUtils.h"

namespace Muninn {

/// A class for calculating different thermodynamic information about the system,
/// based on the estimated density of states. It works by re-weighting the
/// the distribution according to canonical weights
/// \f[
///     w(E) = \exp(-E \beta)
/// \f]
/// where \f$ \beta = 1/(k_B T) \f$. Here \f$ k_B \f$ is Boltzmann constant and
/// \f$ T \f$ is the temperature.
class CanonicalProperties {
public:
    /// Constructor.
    ///
    /// \param bin_centers The energies of the the bin centers; bin_centers(j)
    ///                    should contain the energy in the center of the bin
    ///                    with index j.
    /// \param lnG An estimate of the entropy.
    /// \param lnG_support The support of the entropy estimate.
    CanonicalProperties (const DArray &bin_centers, const DArray &lnG, const BArray &lnG_support) :
        energies(&bin_centers), lnG(&lnG), lnG_support(&lnG_support) {}

    /// Calculates the logarithm of the partition function at a given beta value.
    /// The partition function at \f$ \beta \f$ is given by
    /// \f[
    ///    Z_\beta = \sum_E G(E) \exp(-E \beta)
    /// \f]
    /// where \f$ \beta = 1/(k_B T) \f$.
    /// \param beta The beta value to calculate the partition function at.
    /// \return The log of the partition function at beta, \f$ ln(Z_\beta) \f$.
    inline double lnZ (double beta) {
        return log_sum_exp(*lnG - *energies*beta, *lnG_support);
    }

    /// Calculates the partition function at a given beta value. The partition
    /// function  at \f$ \beta \f$ is given by
    /// \f[
    ///    Z_\beta = \sum_E \exp(-E \beta) G(E)
    /// \f]
    /// where \f$ \beta = 1/(k_B T) \f$.
    ///
    /// \param beta The beta value to calculate the partition function at.
    /// \return The partition function at beta, \f$ ln(Z_\beta) \f$.
    inline double Z (double beta) {
        return exp(lnZ(beta));
    }

    /// Calculates the probability of observing each energy bin, at a given
    /// beta value. The probability of observing then bin with energy
    /// \f$ E \f$ is given by
    /// \f[
    ///    P_\beta(E) = \exp(-E \beta) G(E) Z_\beta^{-1}
    /// \f]
    /// where \f$ Z_\beta \f$ is the partition function at \f$ \beta \f$.
    ///
    /// \param beta The beta value to calculate the distribution at.
    /// \return The array containing the probabilities for the individual bins.
    inline DArray PE (double beta) {
        return TArray_exp<DArray>(*lnG - *energies*beta - lnZ(beta));
    }

    /// Calculates the free energy multiplied by beta. The free
    /// energy a given \f$ \beta \f$ is
    /// \f[
    ///    F(\beta) = -\beta^{-1} \ln(Z_\beta^{-1})
    /// \f]
    /// where \f$ Z_\beta \f$ is the partition function at \f$ \beta \f$.
    ///
    /// \param beta The beta value to distribution at.
    /// \return The free energy multiplied by beta, \f$ F(\beta) \beta \f$.
    inline double betaF (double beta) {
        return -lnZ(beta);
    }

    /// Calculates the average energy at a given beta value. The average energy
    /// at \f$ \beta \f$ is given by
    /// \f[
    ///    \langle E \rangle_\beta = \sum_E E P_\beta(E)
    /// \f]
    /// where \f$ P_\beta(E) \f$ is the probability of the bin with energy
    /// \f$ E \f$ at \f$ \beta \f$.
    ///
    /// \param beta The beta value to calculate the average energy at.
    /// \return The average energy at beta.
    inline double E (double beta) {
        return (*energies*PE(beta)).sum(*lnG_support);
    }

    /// Calculates the average of the square energy at beta. At at given value
    /// of \f$ \beta \f$ this is given by
    /// \f[
    ///    \langle E \rangle_\beta = \sum_E E^2 P_\beta(E)
    /// \f]
    /// where \f$ P_\beta(E) \f$ is the probability of the bin with energy
    /// \f$ E \f$ at \f$ \beta \f$.
    ///
    /// \param beta The beta value to calculate the average at.
    /// \return The average square energy at beta.
    inline double Esq (double beta) {
        return (TArray_pow<DArray>(*energies, 2) * PE(beta)).sum(*lnG_support);
    }

    /// Calculates the entropy divided by the Boltzmann constant at a given
    /// beta value. For a given value of \f$ \beta \f$ this is given by
    /// \f[
    ///    S(\beta) / k_B = \beta (\langle E \rangle_\beta - F(\beta))
    /// \f]
    /// where \f$ T \f$ is the temperature, \f$ \langle E \rangle_\beta \f$ is
    /// the mean energy and \f$ F(\beta) \f$ is the free energy.
    ///
    /// \param beta The beta value to calculate the entropy at.
    /// \return The entropy normalized by the Boltzmann constant, that is
    ///         \f$ S(\beta) / k_B \f$.
    inline double S (double beta) {
        return beta*E(beta) - betaF(beta);
    }

    /// Calculates the heat capacity divided by the Bolztmann constant at a
    /// given beta value. For a given value of \f$ \beta \f$ this is given by
    /// \f[
    ///    C(\beta)/k_B = \beta^2 (\langle E^2 \rangle_\beta - \langle E \rangle_\beta^2)
    /// \f]
    /// where \f$ \langle E \rangle_\beta \f$ is the mean energy and
    /// \f$ \langle E^2 \rangle_\beta \f$ is the mean square energy.
    ///
    /// \param beta The beta value to calculate the heat capacity at.
    /// \return The heat capacity divided by the Boltzmann constant.
    double C (double beta) {
        return std::pow(beta, 2) * (Esq(beta) - std::pow(E(beta), 2));
    }

protected:
    /// Default constructor.
    CanonicalProperties () : energies(NULL), lnG(NULL), lnG_support(NULL) {}

    const DArray *energies;     ///< The energies of the the bin centers; energies(j) is the energy in the center of the bin with index j.
    const DArray *lnG;          ///< An estimate of the entropy.
    const BArray *lnG_support;  ///< The support of the estimated entropy.
};

} // namespace Muninn

#endif /* MUNINN_CANONICAL_PROPERTIES_H_ */

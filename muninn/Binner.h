// Binner.h
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

#ifndef MUNINN_BINNER_H_
#define MUNINN_BINNER_H_

#include <vector>
#include <utility>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimate.h"
#include "muninn/History.h"

namespace Muninn {

/// The Binner abstract class is the interface for binner classes. Binner
/// classes provides an method to partition the energy space into discrete
/// bins.
///
/// Note that binners currently only supports a one dimensional reaction
/// coordinate/energy.
class Binner {
public:
    /// The constructor of the Binner base class.
    Binner(unsigned int nbins, bool uniform) : nbins(nbins), uniform(uniform) {};

    /// Default virtual destructor.
    virtual ~Binner() {};

    /// This method will always be invoked before the calc_bin function. This allows
    /// this initialization of the binner to be based on a set of values.
    ///
    /// \param values The energies for an initial set of samples
    /// \param beta The beta values for the canonical weights used when collecting the initial samples.
    ///             The initial weight are assumed to be of the form \f$ w(E) = \exp(-\beta E) \f$.
    virtual void initialize(std::vector<double> &values, double beta=0.0) = 0;

    /// Function for calculate the bin index for an energy value.
    ///
    /// \param value The energy value, which may or may not be outside the
    ///              binned region
    /// \return The bin index corresponding to the energy value. If the energy
    ///         value falls outside the binned region the return value should be
    ///         negative or equal/larger than the number of bins (Binner#nbins),
    ///         depending on which side of the binned region the values falls.
    ///         The is utilized in the function Binner::calc_bin_validated().
    virtual int calc_bin(double value) const = 0;

    ///  Function for extending the binned region to include a new energy value.
    ///
    /// \param value This energy value should now be included in the binned region.
    /// \param estimate The newest estimate of the density of states.
    /// \param history The current history.
    /// \param lnw The curren log weights
    /// \return The number of bins added in respectively lower and upper end of the histogram
    virtual std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extend(double value, const Estimate &estimate, const History &history, const DArray &lnw) = 0;

    /// Function that returns an array corresponding to the current bin edges.
    ///
    /// \return The current bin edges. The size of the array is one larger than
    ///         the total number of bins (Binner#nbins).
    virtual DArray get_binning() const = 0;

    /// Function that returns an array containing the current values for the
    /// center of the bins. If the edges of bin \f$ i \f$ are \f$ E_{i} \f$ and
    /// \f$ E_{i+1} \f$ then the center of bin \f$ i \f$ is given by
    /// \f$ \frac{E_{i} + E_{i+1}}{2} \f$.
    ///
    /// \return The current bin centers. The size of the array is the same as
    ///         the total number of bins (Binner#nbins).
    virtual DArray get_binning_centered() const = 0;

    /// Function that returns the width of the bins. If the edges of bin
    /// \f$ i \f$ are \f$ E_{i} \f$ and \f$ E_{i+1} \f$ then the bin width for
    /// bin \f$ i \f$ is \f$ E_{i+1}-E_{i} \f$.
    ///
    /// \return The current widths of the bins. The size of the array is the same as
    ///         the total number of bins (Binner#nbins).
    virtual DArray get_bin_widths() const = 0;

    /// Function for checking if a given values fall within the binned region.
    ///
    /// \param value The energy value, which may or may not be outside the
    ///              binned region
    /// \return The first value of the pair gives the bin corresponding to the
    ///         energy value, and the second value is true only if the energy
    ///         falls within the binned region.
    inline std::pair<int, bool> calc_bin_validated(double value) const {
        int bin = calc_bin(value);
        return std::pair<int, bool>(bin, 0<=bin && bin<static_cast<int>(nbins));
    }

    /// Getter function for the current number of bins.
    ///
    /// \return The current number of bins.
    inline unsigned int get_nbins() const {
        return nbins;
    };

    /// Getter function for determining whether the current binning is uniform.
    ///
    /// \return Returns true is the current binning can be assumed to be
    ///         uniform.
    inline bool is_uniform() const {
        return uniform;
    }

protected:
    unsigned int nbins;  ///< The current number of bins
    bool uniform;        ///< Is true if the current binning can be assued to be uniform
};

} // namespace Muninn

#endif /* MUNINN_BINNER_H_ */

// NonUniformBinner.h
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

#ifndef MUNINN_NONUNIFORMBINNER_H_
#define MUNINN_NONUNIFORMBINNER_H_

#include <algorithm>

#include "muninn/common.h"
#include "muninn/Binner.h"

namespace Muninn {

/// A base class for non uniform binning, where the edges of the binned region is
/// stored in an array (NonUniformBinner::binning).
///
/// Note that the class cannot be used directly, as initialize() and extend()
/// are not fully implemented. The class serves as a general base class for
/// non uniform binners.
class NonUniformBinner : public Binner {
public:
    /// Constructor using an initial set of bin edges.
    ///
    /// \param binning The initial edges of the bins.
    NonUniformBinner(const DArray &binning) :
        Binner(binning.get_asize()-1, false, false), binning(binning) {
        assert(binning.get_shape().size() == 1);
    }

    /// Function for calculate the bin index for an energy value. Note that
    /// this call takes log(nbins) time.
    ///
    /// \param value The energy value, which may or may not be outside the
    ///              binned region
    /// \return The bin number corresponding to the energy value. If the energy
    ///         value falls outside the binned region the return value should be
    ///         negative or equal/larger than the number of bins (Binner#nbins),
    ///         depending on which side of the binned region the values falls.
    ///         The is utilized in the function Binner::calc_bin_validated().
    virtual int calc_bin(double value) const {
        // The array is accessed directly, which is not very clean but very fast
        const double* it = std::upper_bound(binning.get_array(), binning.get_array()+binning.get_asize(), value);
        return static_cast<int>(it-binning.get_array()) - 1;
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual void initialize(std::vector<double> &values, double beta=0.0) {
        assert(false);
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extend(double value, const Estimate &estimate, const History &history, const DArray &lnw) {
        assert(false);
        return std::pair<std::vector<unsigned int>, std::vector<unsigned int> >();
    }

    // Implementation of Binner interface (see base class for documentation).
    inline DArray get_binning() const {
        return binning;
    }

    // Implementation of Binner interface (see base class for documentation).
    inline DArray get_binning_centered() const {
        DArray centered(nbins);
        for(unsigned int i=1; i < binning.get_asize(); ++i) {
            centered(i-1) = binning(i-1) + 0.5*(binning(i)-binning(i-1));
        }
        return centered;
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual DArray get_bin_widths() const {
        DArray bin_widths(nbins);
        for(unsigned int i=1; i < binning.get_asize(); ++i) {
            bin_widths(i-1) = binning(i)-binning(i-1);
        }
        return bin_widths;
    }

protected:
    DArray binning; ///< The current edges of the binned region.
};

} // namespace Muninn

#endif /* MUNINN_NONUNIFORMBINNER_H_ */

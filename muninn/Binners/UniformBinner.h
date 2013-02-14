// UniformBinner.h
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

#ifndef MUNINN_UNIFORMBINNER_H_
#define MUNINN_UNIFORMBINNER_H_

#include <algorithm>

#include "muninn/common.h"
#include "muninn/Binner.h"
#include "muninn/utils/utils.h"

namespace Muninn {

/// This class implements simple uniform (linear) binning, where all bins have
/// the same width.
class UniformBinner : public Binner {
public:
    /// Construct where the bin width is set explicit.
    ///
    /// \param bin_width The constant bin width to be used.
    /// \param extend_nbins The number of bins used as additional padding when
    ///                     extending the binned region.
    UniformBinner(double bin_width, unsigned int extend_nbins=20) :
        Binner(0, true), std_bins(0), extend_nbins(extend_nbins),
        min_value(0.0), max_value(0.0), bin_width(bin_width) {};

    /// Constructor where the bin width is set based on assuming that the
    /// samples given to the initialize() function are normal distributed.
    /// The bin with are set based on a given number of bins per +/- one
    /// standard deviation. The standard deviation are estimated from quantiles.
    ///
    /// \param std_bins The number of bins used to describe +/- one standard
    ///                 deviation.
    /// \param extend_nbins The number of bins used as additional padding when
    ///                     extending the binned region.
    UniformBinner(unsigned int std_bins=20, unsigned int extend_nbins=20) :
        Binner(0, true), std_bins(std_bins), extend_nbins(extend_nbins),
        min_value(0.0), max_value(0.0), bin_width(0.0) {};

    /// Constructor where the bin width are set based on an initial min/max
    /// values and a number of bins.
    ///
    /// \param min_value The initial min_value
    /// \param max_value The initial max_value
    /// \param nbins The initial number of bins
    /// \param extend_nbins The number of bins used as additional padding when
    ///                     extending the binned region.
    UniformBinner(double min_value, double max_value, unsigned int nbins, unsigned int extend_nbins=20) :
        Binner(nbins, true), std_bins(std_bins), extend_nbins(extend_nbins),
        min_value(min_value), max_value(max_value) {
        bin_width = (max_value - min_value) / static_cast<double>(nbins);
    };

    /// The initialize works differently depending on which constructor has been
    /// used for this class. In all cases the initializer makes sure that the
    /// bin width is set (if it hasn't been specified in the constructor) and
    /// the range of the binning will included all the values given.
    ///
    /// \param initial_values The energies for an initial set of samples
    /// \param beta The beta values for the canonical weights used when collecting the initial samples.
    ///             The initial weight are assumed to be of the form \f$ w(E) = \exp(-\beta E) \f$.
    virtual void initialize(std::vector<double> &initial_values, double beta=0.0) {
        // If the number of bins has not been set, calculate the number of bins from the initial value
        if (nbins==0) {

            // If the bin width has not been set, it must be estimated using the resolution
            if (bin_width == 0) {
                // Find the quantiles corresponding to +/- a standard deviation
                std::vector<double> quantiles = calculate_fractiles(initial_values, newvector(0.1586553, 0.8413447));
                assert(quantiles[1]-quantiles[0]>0); // TODO: Raise an exception instead of an assert

                // Find the standard deviation and the bin width
                double sigma = 0.5*(quantiles[1]-quantiles[0]);
                bin_width = sigma/static_cast<double>(std_bins);
            }
            // Else, we just need to sort the values (this is done implicit by calculate_fractiles)
            else {
                std::sort(initial_values.begin(), initial_values.end());
            }

            // Find the number of bins
            min_value = initial_values.front() - bin_width/2.0;
            max_value = initial_values.back() + bin_width/2.0;

            nbins = static_cast<unsigned int>((max_value - min_value) / bin_width + 1);
        }
        // If everything has been set up, we just need to make sure
        // that the binning covers the initial samples
        else {
            // Simply call extend on each observation to ensure that it is included in the binning.
            for(std::vector<double>::iterator it = initial_values.begin(); it < initial_values.end(); it++) {
                extend(*it);
            }
        }

        // Print info
        MessageLogger::get().info("Setting bin width to: "+to_string(bin_width));
    }

    /// Function for calculate the bin index for an energy value. This is
    /// done in constant time.
    ///
    /// \param value The energy value, which may or may not be outside the
    ///              binned region
    /// \return The bin index corresponding to the energy value. If the energy
    ///         value falls outside the binned region the return value should be
    ///         negative or equal/larger than the number of bins (Binner#nbins),
    ///         depending on which side of the binned region the values falls.
    ///         The is utilized in the function Binner::calc_bin_validated().
    virtual int calc_bin(double value) const {
        return int( (value-min_value)/bin_width );
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extend(double value, const Estimate &estimate, const History &history, const DArray &lnw) {
        return extend(value);
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual DArray get_binning() const {
        DArray bins(nbins+1);
        for(unsigned int i=0; i<(nbins+1); ++i) {
            bins(i) = min_value + i*bin_width;
        }
        return bins;
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual DArray get_binning_centered() const {
        DArray bins(nbins);
        for(unsigned int i=0; i<nbins; ++i) {
            bins(i) = min_value + i*bin_width + 0.5*bin_width;
        }
        return bins;
    }

    // Implementation of Binner interface (see base class for documentation).
    virtual DArray get_bin_widths() const {
        DArray bin_widths(nbins);
        bin_widths = bin_width;
        return bin_widths;
    }

private:
    unsigned int std_bins;     ///< The number of bins used to describe +/- one standard deviation (only set if the corresponding constructure is used).
    unsigned int extend_nbins; ///< The number of bins used as additional padding when extending the binned region.

    double min_value;          ///< The current minimum value for the binned region.
    double max_value;          ///< The current maximum value for the binned region.
    double bin_width;          ///< The constant bin width


    /// A private version of the extend method, that doesn't take an estimate, history and weights
    ///
    /// \param value This energy value should now be included in the binned region.
    /// \return The number of bins added in respectively lower and upper end of the histogram
    inline std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extend(double value) {
        int bin = calc_bin(value);

        // Check if we are outside the range and calculate how much to add
        std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension(newvector(0u), newvector(0u));

        if (bin < 0) {
            unsigned int to_add = static_cast<unsigned int>(-bin + extend_nbins);
            min_value -= to_add*bin_width;
            nbins += to_add;
            extension.first = newvector(to_add);
        }
        else if (bin >= (int) nbins ) {
            unsigned int to_add = static_cast<unsigned int>(bin-nbins+1 + extend_nbins);
            max_value += to_add*bin_width;
            nbins += to_add;
            extension.second = newvector(to_add);
        }

        return extension;
    }
};

} // namespace Muninn

#endif /* MUNINN_UNIFORMBINNER_H_ */

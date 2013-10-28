// Histogram.h
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

#ifndef MUNINN_HISTOGRAM_H_
#define MUNINN_HISTOGRAM_H_

#include <vector>
#include <deque>
#include <iostream>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

/// The Histogram base class. The class contains a histogram of counts, the
/// weights used to generate the histogram, and the total number of
/// observations in the histogram.
class Histogram {
public:

    /// Constructor for an empty histogram with a given shape.
    ///
    /// \param shape The shape of the histogram.
    Histogram(const std::vector<unsigned int> &shape) :
        N(shape), lnw(shape), n(0), shape(shape) {}

    /// Constructor for an histogram with a set of weights. The count histogram
    /// will be empty, but the histogram will get the same shape as the weights.
    ///
    /// \param lnw The weights the histogram will be initialized with.
    Histogram(const DArray &lnw) :
        N(lnw.get_shape()), lnw(lnw), n(0), shape(lnw.get_shape()) {}

    /// Constructor for an histogram with a initial set of counts and a set of
    /// corresponding weights. The count histogram.
    ///
    /// \param N The initial set of counts.
    /// \param lnw The weights the histogram will be initialized with.
    Histogram(const CArray &N, const DArray &lnw) :
        N(N), lnw(lnw), n(N.sum()), shape(N.get_shape()) {
    	assert(N.same_shape(lnw));
    }

    /// Default destructor
    virtual ~Histogram() {}

    /// Function for adding a one dimensional observation to the histogram.
    ///
    /// \param bin The bin index for the observation.
    virtual inline void add_observation(unsigned int bin) {
        N(bin)++;
        n++;
    }

    /// Function for adding a two dimensional observation to the histogram.
    ///
    /// \param bin1 The first bin index of the observation to be added.
    /// \param bin2 The second bin index of the observation to be added.
    virtual inline void add_observation(unsigned int bin1, unsigned int bin2) {
        N(bin1, bin2)++;
        n++;
    }

    /// Function for adding multidimensional observations to the histogram.
    ///
    /// \param bin The multidimensional index of the bin for the observation.
    virtual inline void add_observation(std::vector<unsigned int> &bin) {
        N(bin)++;
        n++;
    }

    /// Function for extending the shape of the Histogram.
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    virtual void extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
        N = N.extended(add_under, add_over);
        lnw = lnw.extended(add_under, add_over);
        shape = add_vectors(shape, add_under, add_over);
    }

    /// Set the weights used to collect the histogram.
    ///
    /// \param new_lnw The new log weights.
    void set_lnw(const DArray &new_lnw) {
        assert(new_lnw.has_shape(shape));
        lnw=new_lnw;
    }

    /// Getter for the histogram counts.
    ///
    /// \return The histogram counts.
    inline const CArray& get_N() const {return N;}

    /// Getter for the weights used to collect the histogram.
    ///
    /// \return The weights used to collect the histogram.
    inline const DArray& get_lnw() const {return lnw;}

    /// Getter for the total number of counts collected in the histogram.
    ///
    /// \return The total number of counts collected in the histogram.
    inline Count get_n() const {return n;}

    /// Get the shape of the histogram.
    ///
    /// \return The shape of the histogram.
    inline const std::vector<unsigned int>& get_shape() const {return shape;}

    // Define output strem operator as friend
    friend std::ostream &operator<<(std::ostream &output, const Histogram &histogram);

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const {
        statistics_logger.add_entry("N", N);
        statistics_logger.add_entry("lnw", lnw);
    }

private:
    CArray N;                        ///< The counts/observations.
    DArray lnw;                      ///< The weights used for collecting the histogram.
    Count n;                         ///< The total number of observations.
    std::vector<unsigned int> shape; ///< The shape of the histogram.
};

/// Output stream operator for the Muninnn Histogram class.
inline std::ostream &operator<<(std::ostream &output, const Histogram &histogram) {
    output << "[Histogram]\n";
    output << "N = " << histogram.N << std::endl;
    output << "lnw = " << histogram.lnw << std::endl;
    output << "n = " << histogram.n << std::endl;
    return output;
}

} // namespace Muninn

#endif /* MUNINN_HISTOGRAM_H_ */

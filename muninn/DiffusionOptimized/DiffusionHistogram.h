// DiffusionHistogram.h
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

#ifndef MUNINN_DIFFUSION_HISTOGRAM_H_
#define MUNINN_DIFFUSION_HISTOGRAM_H_

#include <vector>
#include <deque>
#include <iostream>

#include "muninn/common.h"
#include "muninn/Histogram.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

class DiffusionHistogram : public Histogram, public BaseConverter<Histogram, DiffusionHistogram> {
public:

    DiffusionHistogram(const std::vector<unsigned int> &shape) :
        Histogram(shape) {}

    DiffusionHistogram(const DArray &lnw) :
        Histogram(lnw) {}

    DiffusionHistogram(const CArray &N, const DArray &lnw) :
        Histogram(N, lnw) {}

    /// Default destructor
    virtual ~DiffusionHistogram() {}

    /// Function for adding a one dimensional observation to the histogram.
    ///
    /// \param bin The bin index for the observation.
    virtual inline void add_observation(unsigned int bin) {
        Histogram::add_observation(bin);
        // To P&W: You should add some code here
    }

    /// Function for adding a two dimensional observation to the histogram.
    ///
    /// \param bin1 The first bin index of the observation to be added.
    /// \param bin2 The second bin index of the observation to be added.
    virtual inline void add_observation(unsigned int bin1, unsigned int bin2) {
        Histogram::add_observation(bin1, bin2);
        // To P&W: You should add some code here
    }

    /// Function for adding multidimensional observations to the histogram.
    ///
    /// \param bin The multidimensional index of the bin for the observation.
    virtual inline void add_observation(std::vector<unsigned int> &bin) {
        Histogram::add_observation(bin);
        // To P&W: You should add some code here
    }

    /// Function for extending the shape of the Histogram.
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    virtual void extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
        Histogram::extend(add_under, add_over);
        // To P&W: You should add some code here
    }

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const {
        Histogram::add_statistics_to_log(statistics_logger);
        // To P&W: You should add some code here
    }

private:
};

} // namespace Muninn

#endif /* DIFFUSION_MUNINN_HISTOGRAM_H_ */

// MessageException.h
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

#ifndef MUNINN_MAXIMALNUMBEROFBINSEXCEED_H_
#define MUNINN_MAXIMALNUMBEROFBINSEXCEED_H_

#include <exception>
#include <string>

#include "muninn/Exceptions/MessageException.h"

namespace Muninn {

/// This exception is thrown, when an binner exceeds the maximal number of
/// allowed bins.
class MaximalNumberOfBinsExceed : public MessageException {
public:
    /// Constructor
    ///
    /// \param max_number_of_bins The maximal number of bins the binner can use.
    /// \param requested_number_of_bins The number of bins requested by the binner.
    MaximalNumberOfBinsExceed(unsigned int max_number_of_bins, unsigned int requested_number_of_bins) throw() :
        MessageException(std::string("The binner tried to exceed the maximal number of bins (max:" + to_string(max_number_of_bins) + ", requested:" + to_string(requested_number_of_bins) + ").")),
        max_number_of_bins(max_number_of_bins), requested_number_of_bins(requested_number_of_bins) {};

    /// Virtual destructor
    virtual ~MaximalNumberOfBinsExceed() throw() {}

    /// Getter for the maximal number of bins
    unsigned int get_max_number_of_bins() {
        return max_number_of_bins;
    }

    /// Getter for the requested number of bins
    unsigned int get_requested_number_of_bins() {
        return requested_number_of_bins;
    }

private:
    unsigned int max_number_of_bins;       ///< The maximal number of bins the binner can use.
    unsigned int requested_number_of_bins; ///< The number of bins requested by the binner.
};

} // namespace Muninn

#endif /* MUNINN_MAXIMALNUMBEROFBINSEXCEED_H_ */

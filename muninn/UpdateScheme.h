// UpdateScheme.h
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

#ifndef MUNINN_UPDATESCHEME_H_
#define MUNINN_UPDATESCHEME_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/History.h"
#include "muninn/Histogram.h"
#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

/// The UpdateScheme abstract class is the interface for classes implementing
/// update schemes. An update scheme determines weather it is time to make a
/// new estimate of the entropy and update the weights.
///
/// All update schemes must have an initial_max, this is number of iterations used
/// by the GE for the for the collection of the first histogram.
class UpdateScheme : public Loggable {
public:

    /// Default constructor for an update scheme
    ///
    /// \param initial_max The maximal number of iterations the first (initial) histogram.
    UpdateScheme(Count initial_max) : initial_max(initial_max) {}

    /// Default virtual destructor.
    virtual ~UpdateScheme() {};

    /// Checks if the entropy estimate and the weights should be updated.
    ///
    /// \param current The histogram for the current (ongoing) simulation.
    /// \param history The history of previous histograms
    /// \return Returns true if the weights should be updated.
    virtual bool update_required(const Histogram &current, const History &history)=0;

    /// This function is always called before the current histogram is added
    /// to the history. This allows the update scheme to updates its state.
    ///
    /// \param current The current histogram that will be added to the history.
    /// \param history The history to which, the current histogram will be added.
    virtual void updating_history(const Histogram &current, const History &history)=0;

    /// Getter for initial_max, which is the maximal number of iterations the
    /// first (initial) histogram.
    ///
    /// \return Returns initial_max.
    const Count& get_initial_max() const {
        return initial_max;
    }

    /// Tell the updated scheme to prolong the simulation time for the current
    /// histogram. This function is normally called if the estimator failed to
    /// estimate the entropy based on the current histogram and history. It should
    /// be possibly to call the function successively.
    virtual void prolong()=0;

    /// Reset all previous calls to prolong.
    virtual void reset_prolonging()=0;

    // TODO: A more elegant alternative to prolong and unprolong should be introduced.

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const {}

private:
    const Count initial_max; ///< The maximal number of iterations the first (initial) histogram.
};

} // namespace Muninn

#endif /* MUNINN_UPDATESCHEME_H_ */

// IncreaseFactorScheme.h
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

#ifndef MUNINN_INCREASEFACTORSCHEME_H_
#define MUNINN_INCREASEFACTORSCHEME_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/History.h"
#include "muninn/Histogram.h"
#include "muninn/Histories/MultiHistogramHistory.h"

namespace Muninn {

/// In the IncreaseFactorScheme the simulation time for each round grows
/// exponentially under a condition that the support does not increase.
/// If \f$ \tau_{k} \f$ is the simulation time for the k'th round, the simulation
/// time for the (k+1)'th round is given by
/// \f[
///     \tau_{k+1} =
///        \left\{
///        \begin{array}{ll}
///           \gamma\tau_{k}  & \textrm{if the support of the (k-1)'th histogram has no increased more than a given fraction compared to the history} \\ \null
///           \tau_{k}        & \textrm{otherwise}
///        \end{array}
///        \right.
/// \f]
/// A bin in a histogram is said to have support, if it has at least
/// IncreaseFactorScheme::min_count observations. The history has support in a
/// bin, if a least one histogram in the history has support in the bin.
class IncreaseFactorScheme : public UpdateScheme {
public:

    /// The constructor for the update scheme.
    ///
    /// \param initial_max The sampling time (number of iteration) for the first
    ///                    (initial) round of sampling.
    /// \param increase_factor The sampling time is multiplied with this factor
    ///                        if the size of the support is not increased for
    ///                        the new histogram.
    /// \param min_count The support of a histogram is defined as the bins with
    ///                  a least min_count observations.
    /// \param fraction If the number of new bins with support in the new histogram
    ///                 is less than this fraction of the size of the support of
    ///                 the current history, the simulation time is multiplied by
    ///                 the increase_factor. If the factor is negative, the
    ///                 simulation time is only increased if no new bins
    ///                 has gained support.
    IncreaseFactorScheme(Count initial_max = 5000, double increase_factor = 1.07, Count min_count = 20, double fraction=0.05) :
        UpdateScheme(initial_max), this_max(initial_max), prolonging(0), increase_factor(increase_factor), min_count(min_count), fraction(fraction) {}

    /// Checks if the entropy estimate and the weights should be updated. An
    /// updated is required if the total number of observations in the
    /// current histogram has reached the current IncreaseFactorScheme::this_max.
    ///
    /// \param current The histogram for the current (ongoing) simulation.
    /// \param history The history of previous histograms
    /// \return Returns true if the weights should be updated.
    virtual bool update_required(const Histogram &current, const History &history) {
        return current.get_n() >= (this_max+prolonging);
    }

    /// This function is always called before the current histogram is added
    /// to the history. This allows the the scheme to update the value of the
    /// current IncreaseFactorScheme::this_max.
    ///
    /// \param current The current histogram that will be added to the history.
    /// \param history The history to which, the current histogram will be added.
    virtual void updating_history(const Histogram &current, const History &history) {
        const MultiHistogramHistory& mh_history = MultiHistogramHistory::cast_from_base(history, "The FractionalIncreaseFactorScheme update scheme is only compatible with the MultiHistogramHistory.");

        if (mh_history.get_size() > 0) {
            // Check if thismax should be updated
            BArray prev_observed(mh_history.get_shape());

            for (MultiHistogramHistory::const_iterator it=mh_history.begin(); it!=mh_history.end(); it++)
                prev_observed = prev_observed || ((*it)->get_N() >= min_count);

            unsigned int num_prev_observed = number_of_true(prev_observed);

            BArray new_observations = (current.get_N() >= min_count) && (!prev_observed);
            unsigned int new_observed_bins = number_of_true(new_observations);

            if (new_observed_bins<fraction*num_prev_observed || (new_observed_bins==0 && fraction<0) ) {
                this_max = static_cast<Count>(this_max * increase_factor);
                MessageLogger::get().debug("Settings this_max to "+to_string(this_max)+".");
            }
        }
    }

    /// Tell the updated scheme to prolong the simulation time for the current
    /// histogram. For this update scheme the simulation time is prolonged by
    /// 1/4 of the current value of IncreaseFactorScheme::this_max.
    virtual void prolong() {
        prolonging += this_max/4;
    }

    /// Reset all previous calls to prolong.
    virtual void reset_prolonging() {
        prolonging = 0;
    }

    /// Get the required number of iterations for the current round.
    ///
    /// \return The required number of iterations for the current round.
    Count get_this_max() const {
        return this_max;
    }

private:
    Count this_max;          ///< The required number of iterations required for completing the current round.
    Count prolonging;        ///< Number of iterations the current round is prolonged by.
    double increase_factor;  ///< The factor this_max is increased by if the support has not grown more than fraction.
    Count min_count;         ///< The support of a histogram is defined as the bins with a least min_count observations.
    double fraction;         ///< If the number of new bins with support in the new histogram is less than this fraction of the size of the support of the current history, the simulation time is increased.
};

} // namespace Muninn

#endif /* MUNINN_INCREASEFACTORSCHEME_H_ */

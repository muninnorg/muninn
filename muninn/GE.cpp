// GE.cpp
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

#include <deque>
#include <vector>

#include "muninn/GE.h"

namespace Muninn {

void GE::estimate_new_weights(const Binner *binner) {
    // Inform that new weights will be estimated
    MessageLogger::get().info("Estimating new weights.");
    MessageLogger::get().debug("Histogram shape: " + to_string<std::vector<unsigned int> >(current_sum->get_shape()));

    // Bookkeeping
    total_iterations += current_sum->get_n();

    // Tell the update scheme that the history is to be updated
    // TODO: Find a more elegant way of doing this.
    updatescheme->updating_history(*current_sum, *history);

    int failed_at_thread_id = -1;

    for (unsigned int thread_id=0; thread_id < current_histograms.size(); ++thread_id) {
        MessageLogger::get().info("Thread ID " + to_string<unsigned int>(thread_id));

        if (current_histograms.at(thread_id)->get_n() > 0) {
            // Put the current histogram into the history.
            history->add_histogram(current_histograms.at(thread_id));
            current_histograms.at(thread_id) = NULL;

            try {
                // Estimate lnG from the data
                estimator->estimate(*history, *estimate, binner);

                // Log the current statistics
                force_statistics_log();
            }
            catch (EstimatorException &exception){
                // Write warnings
                MessageLogger::get().warning(exception.what());
                MessageLogger::get().warning("Keeping old weights.");

                // Store which thread id the estimation failed at
                failed_at_thread_id = thread_id;
                break;
            }
        }
        else {
            MessageLogger::get().debug("Skipping estimation due to empty histogram");
        }
    }

    if (failed_at_thread_id < 0) {
        // Clean the current sum histogram and empty histograms in current histograms
        delete current_sum;
        for (unsigned int thread_id=0; thread_id<current_histograms.size(); ++thread_id) {
            delete current_histograms.at(thread_id);
            current_histograms.at(thread_id) = NULL;
        }

        // Make a new empty current histograms, with the newly estimated weights
        DArray new_weights = weightscheme->get_weights(*estimate, *history, binner);

        current_sum = estimator->new_histogram(new_weights);

        for (unsigned int thread_id=0; thread_id<current_histograms.size(); ++thread_id) {
            current_histograms.at(thread_id) = estimator->new_histogram(new_weights);
        }

        // TODO: Find a more elegant way of doing this.
        updatescheme->reset_prolonging();
    }
    else {
        // Clean up the history and prolong the simulation time
        // TODO: Find a more elegant way of doing this.
        for (int thread_id=failed_at_thread_id; thread_id >= 0; --thread_id) {
            if(current_histograms.at(thread_id) == NULL) {
                current_histograms.at(thread_id) = history->remove_newest();
            }
        }
        updatescheme->prolong();
    }
}

void GE::extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over, const Binner *binner) {
    // TODO: Check that a maximum number of bins has not been exceeded

    // Extend the the current histogram and the history
    current_sum->extend(add_under, add_over);

    for (unsigned int thread_id=0; thread_id<current_histograms.size(); ++thread_id) {
        current_histograms.at(thread_id)->extend(add_under, add_over);
    }

    history->extend(add_under, add_over);

    // Extend the estimate
    estimator->extend_estimate(*history, *estimate, add_under, add_over);

    // Set the new weights based on the new estimate
    DArray new_weights = weightscheme->get_weights(*estimate, *history, binner);
    current_sum->set_lnw(new_weights);
    for (unsigned int thread_id=0; thread_id<current_histograms.size(); ++thread_id) {
        current_histograms.at(thread_id)->set_lnw(new_weights);
    }

}

} // namespace Muninn

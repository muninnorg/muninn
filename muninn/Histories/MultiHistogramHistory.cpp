// MultiHistogramHistory.cpp
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

#include <cassert>

#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/common.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"

namespace Muninn {

const std::string MultiHistogramHistory::history_mode_names[] = {"drop-none", "drop-oldest", "drop-oldest-possible", "drop-any-possible"};

void MultiHistogramHistory::add_histogram(const Histogram &histogram) {
    // Check that the histogram has the correct shape
    assert(vector_equal(histogram.get_shape(), this->shape));

    // Add the histogram to the deque
    histograms.push_front(new Histogram(histogram));

    // Update sum_N
    sum_N += histograms.front()->get_N();

    switch (history_mode) {
    case DROP_NONE : {}
    break;

    case DROP_OLDEST : {
        // Check if the last histogram should be removed
        while (histograms.size() > memory) {
            remove_last_histogram();
        }
    }
    break;

    case DROP_OLDEST_POSSIBLE : {
        // Check if the last histogram should be removed
        while (histograms.size() > memory) {
            // Find the sum of N and support
            BArray current_support = sum_N >= min_count;
            CArray remaining_sum_N = sum_N-histograms.back()->get_N();
            BArray remaining_support = remaining_sum_N >= min_count;

            // Find the number of bins that are observed in the oldest histogram but not in the remaining
            unsigned int overlap = number_of_true(current_support && (!remaining_support));

            if (overlap==0) {
                remove_last_histogram();
            }
            else {
                MessageLogger::get().warning("To preserve the support the number of histograms in the history are: " + to_string(histograms.size()));
                break;
            }
        }
    }
    break;

    case DROP_ANY_POSSIBLE : {
        // See if some of the oldest histograms can be deleted
        std::deque<Histogram*>::iterator it = histograms.end()-1;

        // Check if the last histogram should be removed
        while ((it-histograms.begin()) > static_cast<int>(memory)) {
            // Find the sum of N and support
            BArray current_support = sum_N>=min_count;
            CArray remaining_sum_N = sum_N-(*it)->get_N();
            BArray remaining_support = remaining_sum_N>=min_count;

            // Find the number of bins that are observed in the oldest histogram but not in the remaining
            unsigned int overlap = number_of_true(current_support && (!remaining_support));

            // Delete the histogram, if there is no overlap
            if (overlap==0) {
                // Update sum_N and remove the histogram
                sum_N -= (*it)->get_N();
                delete *it;
                it = histograms.erase(it);
            }

            // Move to the previous histogram
            --it;
        }

        if (histograms.size()>memory) {
            MessageLogger::get().warning("To preserve the support the number of histograms in the history are: " + to_string(histograms.size()));
        }
    }
    break;

    case SIZE:
    default:
        throw MessageException("Unsupported mode for MultiHistogramHistory.");
    }
}

void MultiHistogramHistory::extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over) {
    // Called method in base class
    History::extend(add_under, add_over);

    // Extend the histograms
    for(std::deque<Histogram*>::iterator it = histograms.begin(); it != histograms.end(); it++) {
        (*it)->extend(add_under, add_over);
    }

    // Extend sum_N
    sum_N = sum_N.extended(add_under, add_over);
}

// TODO: ! Find solution to these methods beeing removed
//std::vector<const CArray*> MultiHistogramHistory::get_Ns() const {
//    std::vector<const CArray*> Ns;
//    for(std::deque<Histogram*>::const_iterator it = histograms.begin(); it != histograms.end(); it++) {
//        Ns.push_back(&((*it)->get_N()));
//    }
//    return Ns;
//}
//
//std::vector<const DArray*> MultiHistogramHistory::get_lnws() const {
//    std::vector<const DArray*> lnws;
//    for(std::deque<Histogram*>::const_iterator it = histograms.begin(); it != histograms.end(); it++) {
//        lnws.push_back(&((*it)->get_lnw()));
//    }
//    return lnws;
//}
//
//std::vector<Count> MultiHistogramHistory::get_ns() const {
//    std::vector<Count> ns;
//    for(std::deque<Histogram*>::const_iterator it = histograms.begin(); it != histograms.end(); it++) {
//        ns.push_back((*it)->get_n());
//    }
//    return ns;
//}


void MultiHistogramHistory::remove_last_histogram() {
    // Update sum_N
    sum_N -= histograms.back()->get_N();

    // Remove the histogram
    delete histograms.back();
    histograms.pop_back();
}

void MultiHistogramHistory::remove_newest() {
    if (histograms.size() > 0) {
        // Update sum_N
        sum_N -= histograms.front()->get_N();

        // Remove the histogram
        delete histograms.front();
        histograms.pop_front();
    }
}

void MultiHistogramHistory::add_statistics_to_log(StatisticsLogger& statistics_logger) const {
    for (const_reverse_iterator it=rbegin(); it!=rend(); ++it) {
        (*it)->add_statistics_to_log(statistics_logger);
    }
}

std::istream &operator>>(std::istream &input, MultiHistogramHistory::HistoryMode &mode) {
    std::string raw_string;
     input >> raw_string;

     for (unsigned int i=0; i<MultiHistogramHistory::SIZE; ++i) {
          if (raw_string == MultiHistogramHistory::history_mode_names[i]) {
               mode = MultiHistogramHistory::HistoryMode(i);
          }
     }

     return input;
}

std::ostream &operator<<(std::ostream &o, const MultiHistogramHistory::HistoryMode &mode) {
     o << Muninn::MultiHistogramHistory::history_mode_names[static_cast<unsigned int>(mode)];
     return o;
}

} // namespace Muninn

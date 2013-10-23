// MultiHistogramHistory.h
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

#ifndef MUNINN_MULTIHISTOGRAMHISTORY_H_
#define MUNINN_MULTIHISTOGRAMHISTORY_H_

#include <vector>
#include <deque>
#include <iostream>

#include "muninn/common.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/TArray.h"
#include "muninn/History.h"
#include "muninn/Histogram.h"
#include "muninn/utils/BaseConverter.h"

namespace Muninn {

/// The MultiHistogramHistory class is a history that can store multiple
/// consecutive histograms in the memory.
class MultiHistogramHistory : public History, public BaseConverter<History, MultiHistogramHistory>  {
public:
    /// Define the modes for handling the history with respected to deletion of
    /// old histograms. A string representation is given by #history_mode_names.
    enum HistoryMode {
    	DROP_NONE,            ///< Do not drop any histogram.
        DROP_OLDEST,          ///< Always drop the oldest histogram outside the memory size.
        DROP_OLDEST_POSSIBLE, ///< Drop the oldest histograms outside the memory size, if this does not decrease the size of the support.
        DROP_ANY_POSSIBLE,    ///< Drop any of the oldest histograms outside the memory size, if this does not does not decrease the size of the support.
        SIZE                  ///< Indicator value.
    };

    /// A string representation of the enum #HistoryMode.
    static const std::string history_mode_names[];

    /// Constructor.
    ///
    /// \param shape The initial shape of the history
    /// \param memory The number of histograms to keep in memory.
    /// \param min_count The minimal number of counts for a bin to have support.
    /// \param history_mode Describes the procedure for deleting old histograms.
    MultiHistogramHistory(const std::vector<unsigned int> &shape, unsigned int memory, Count min_count, HistoryMode history_mode) :
        History(shape), memory(memory), min_count(min_count), history_mode(history_mode), sum_N(shape) {}

    /// Destructor.
    virtual ~MultiHistogramHistory() {
        for (std::deque<Histogram*>::iterator it=histograms.begin(); it!=histograms.end(); ++it) {
            delete *it;
        }
    }

    /// Function for adding a histogram to the history. The histogram is
    /// always added to the front of the internal deque
    /// MultiHistogramHistory::histograms.
    ///
    /// \param histogram The histogram to be added.
    virtual void add_histogram(const Histogram &histogram);

    /// Function for extending the shape of the History. When this functions is
    /// called all histograms in the internal deque MultiHistogramHistory::histograms
    /// are extended, which makes the function rather expensive.
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    virtual void extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over);

    /// Remove the newest histogram from the history. In this case, the
    /// histogram in front of the internal deque MultiHistogramHistory::histograms
    /// is removed.
    virtual void remove_newest();

    /// This function overloads the []-operator and gives access to the
    /// individual histograms. The function is safe in the sense, that it uses
    /// the deque function std::deque::at, which throws a out_of_range
    /// exception, if the the index is out of range.
    ///
    /// \param i The index of the histogram to access.
    /// \return The i'th histogram.
    inline const Histogram& operator[](unsigned int i) const {return *histograms.at(i);}

    /// Function for getting the sum of counts in each bin, called the sum
    /// histogram. The returned array has the same shape as the individual
    /// histograms, and the \f$ i \f$'th bin of the sum histogram
    /// \f$ \mathrm{sum\_n} \f$ is
    ///
    /// \f[
    ///  \mathrm{sum\_n}[i] = \sum_j N_j[i]
    /// \f]
    ///
    /// where \f$ N_j \f$ is the \f$ j \f$'th histogram.
    ///
    /// \return The sum histogram.
    inline const CArray& get_sum_N() const {return sum_N;}

    /// Get a vector containing pointer to the individual count arrays from the
    /// list of histograms. Note that a new vector constructed each time the
    /// function is called.
    ///
    /// \return A vector of pointer to count arrays.
//    std::vector<const CArray*> get_Ns() const;

    /// Get a vector containing pointer to the individual weight arrays from
    /// the list of histograms. Note that a new vector constructed each time
    /// the function is called.
    ///
    /// \return A vector of pointer to weight arrays.
//    std::vector<const DArray*> get_lnws() const;

    /// Get a vector containing the sum of counts in the individual histograms.
    /// Note that a new vector constructed each time the function is called, but
    /// no calculations are done.
    ///
    /// \return A vector containing the sum of counts in the individual
    ///         histograms.
//    std::vector<Count> get_ns() const;

    /// Get the number of Histograms in the history.
    ///
    /// \return The number of histograms in the history.
    inline unsigned int get_size() const {return histograms.size();}

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const;

    /// Type of forward iterator for the MultiHistogramHistory.
    typedef std::deque<Histogram*>::iterator iterator;

    /// Type of constant forward iterator for the MultiHistogramHistory.
    typedef std::deque<Histogram*>::const_iterator const_iterator;

    /// Type of reverse iterator for the MultiHistogramHistory.
    typedef std::deque<Histogram*>::reverse_iterator reverse_iterator;

    /// Type of reverse iterator for the MultiHistogramHistory.
    typedef std::deque<Histogram*>::const_reverse_iterator const_reverse_iterator;

    /// \return Forward iterator pointing to the first element in the history.
    inline iterator begin() {return histograms.begin();}

    /// \return Forward iterator pointing one past the last element in the history.
    inline iterator end() {return histograms.end();}

    /// \return Constant forward iterator pointing to the first element in the history.
    inline const_iterator begin() const {return histograms.begin();}

    /// \return Constant forward iterator pointing one past the last element in the history.
    inline const_iterator end() const {return histograms.end();}

    /// \return Reverse iterator pointing to the last element in the history.
    inline reverse_iterator rbegin() {return histograms.rbegin();}

    /// \return Reverse iterator pointing one before the first element in the history.
    inline reverse_iterator rend() {return histograms.rend();}

    /// \return Constant reverse iterator pointing one before the first element in the history.
    inline const_reverse_iterator rbegin() const {return histograms.rbegin();}

    /// \return Constant iterator pointing one before the first element in the history.
    inline const_reverse_iterator rend() const {return histograms.rend();}

    // Friends (input and output operator for the HistoryMode)
    friend std::istream &operator>>(std::istream &input, HistoryMode &mode);
    friend std::ostream &operator<<(std::ostream &o, const HistoryMode &mode);

private:
    const unsigned int memory;          ///< The maximal length of the history.
    const Count min_count;              ///< The minimal number of counts is used to determine if the last histogram should be removed.
    const HistoryMode history_mode;     ///< The mode for removing histograms from the history.
    std::deque<Histogram*> histograms;  ///< The histograms in the history.
    CArray sum_N;                       ///< The sum of counts in each bin across all histograms.

    /// Removed the last (oldest) histogram from the history.
    void remove_last_histogram();
};

/// Input operator for MultiHistogramHistory::HistoryMode.
std::istream &operator>>(std::istream &input, MultiHistogramHistory::HistoryMode &mode);

/// Output operator for MultiHistogramHistory::HistoryMode.
std::ostream &operator<<(std::ostream &o, const MultiHistogramHistory::HistoryMode &mode);

} // namespace Muninn

#endif /* MUNINN_MULTIHISTOGRAMHISTOGRAMS_H_ */

// StatisticsLogReader.h
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

#ifndef MUNINN_STATISTICS_LOG_READER_H
#define MUNINN_STATISTICS_LOG_READER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

namespace Muninn {

/// Class for reading the contents of a statistics log file (written by the
/// StatisticsLogger). The data is accessible as TArrays.
class StatisticsLogReader {
public:

    /// Constructor for the log reader.
    ///
    /// \param filename The filename of the log file to read.
    /// \param max_hist Read maximal this number of entries from the log file,
    ///                 starting from the beginning. A values of zero means
    ///                 reading the whole file.
    StatisticsLogReader(const std::string & filename, unsigned int max_hist=0) : max_hist(max_hist) {
        // Open the input file
        std::ifstream input(filename.c_str());
        if (input.fail()) {
            throw MessageException("Could not open statics logfile.");
        }

        // Read the input file
        read(input);
        input.close();
    }

    /// Get a vector containing the read ids and histograms.
    ///
    /// \return A vector of ids and histograms. The i'th entry in the vector,
    ///         Ns[i], contains the id (Ns[i].first) and the array
    ///         (Ns[i].second) for the i'th histogram.
    const std::vector<std::pair<std::string,CArray> >& get_Ns() const {
        return Ns;
    }

    /// Get a vector containing the read ids and log weights.
    ///
    /// \return A vector of ids and log weights. The i'th entry in the vector,
    ///         lnws[i], contains the id (lnws[i].first) and the array
    ///         (lnws[i].second) for the i'th weights.
    const std::vector<std::pair<std::string,DArray> >& get_lnws() const {
        return lnws;
    }

    /// Get a vector containing the read ids and entropy estimates (lnG).
    ///
    /// \return A vector of ids and entropy estimates. The i'th entry in the
    ///         vector, lnGs[i], contains the id (lnGs[i].first) and the array
    ///         (lnwG[i].second) for the i'th weights.
    const std::vector<std::pair<std::string,DArray> >& get_lnGs() const {
        return lnGs;
    }

    /// Get a vector containing the read ids and the support of the entropy
    /// estimates.
    ///
    /// \return A vector of ids and supports of the entropy estimates. The i'th
    ///         entry in the vector, lnG_supports[i], contains the id
    ///         (lnG_supports[i].first) and the array (lnG_supports[i].second)
    ///         for the i'th weights.
    const std::vector<std::pair<std::string,BArray> >& get_lnG_supports() const {
        return lnG_supports;
    }

    /// Get a vector containing the read ids and binning arrays. A binning
    /// arrays contains the edge values of the bins.
    ///
    /// \return A vector of ids and binning arrays. The i'th
    ///         entry in the vector, binnings[i], contains the id
    ///         (binnings[i].first) and the array (binnings[i].second)
    ///         for the i'th binning.
    const std::vector<std::pair<std::string,DArray> >& get_binnings() const {
        return binnings;
    }

    /// Get a vector containing the read ids and bin widths.
    ///
    /// \return A vector of ids and bin width arrays. The i'th
    ///         entry in the vector, binnings[i], contains the id
    ///         (bin_widths[i].first) and the array (bin_widths[i].second)
    ///         for the i'th bin widths.
    const std::vector<std::pair<std::string,DArray> >& get_bin_widths() const {
        return bin_widths;
    }

private:
    unsigned int max_hist;                                      ///< The maximal this number of entries from the log file, starting from the beginning of the file.

    std::vector<std::pair<std::string,CArray> > Ns;             ///< The read vector of ids and histograms.
    std::vector<std::pair<std::string,DArray> > lnws;           ///< The read vector of ids and log weights.
    std::vector<std::pair<std::string,DArray> > lnGs;           ///< The read vector of ids and entropy estimates.
    std::vector<std::pair<std::string,BArray> > lnG_supports;   ///< The read vector of ids and support of the entropy estimates.
    std::vector<std::pair<std::string,DArray> > binnings;       ///< The read vector of ids and bin edge arrays.
    std::vector<std::pair<std::string,DArray> > bin_widths;     ///< The read vector of ids and bin width arrays.

    /// Read the arrays for an input stream.
    ///
    /// \param input The input stream to read from.
    void read(std::istream & input);

    /// Convert a sequence of (std::string, std::string)-pairs to a sequence
    /// (std::string, ARRAY)-pairs.
    ///
    /// \param from The input sequence of (std::string, std::string)-pairs.
    /// \param to The output sequence of (std::string, ARRAY)-pairs.
    ///
    /// \tparam ARRAY This type is assumed to have a read function that takes an
    ///               input stream as argument as for instance TArray::read.
    template <class ARRAY>
    inline static void string_to_array(std::deque<std::pair<std::string,std::string> > &from, std::vector<std::pair<std::string, ARRAY> > &to) {
        for (std::deque<std::pair<std::string,std::string> >::const_iterator it=from.begin(); it!=from.end(); ++it) {
            std::pair<std::string, ARRAY> values;
            values.first = it->first;
            values.second.read(it->second);
            to.push_back(values);
        }
    }
};

} // namespace Muninn

#endif // MUNINN_STATISTICS_LOG_READER_H

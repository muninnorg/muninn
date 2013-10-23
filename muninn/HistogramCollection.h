// HistoryCollection.h
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

// TODO: Document this file

#ifndef MUNINN_HISTOGRAMCOLLECTION_H_
#define MUNINN_HISTOGRAMCOLLECTION_H_

#include "muninn/Histogram.h"


namespace Muninn {

class HistogramCollection {
public:
    HistogramCollection(const std::vector<unsigned int> &shape) : histograms(), shape(shape), sum_N(shape), sum_n(0) {};

    void add_histogram(const Histogram &histogram) {
        // Check that the histogram has the correct shape
        assert(vector_equal(histogram.get_shape(), this->shape));

        // Update sum_N and add the histogram to the vector of histograms
        sum_n += histogram.get_n();
        sum_N += histogram.get_N();
        histograms.push_back(new Histogram(histogram));
    }

    /// Function for adding a one dimensional observation to a histogram in the collection.
    ///
    /// \param bin The bin index for the observation.
    inline void add_observation(unsigned int bin, unsigned int index) {
        assert(index < histograms.size());
        histograms[index]->add_observation(bin);
        sum_N(bin)++;
        sum_n++;
    }

    /// Function for adding a two dimensional observation to the histogram.
    ///
    /// \param bin1 The first bin index of the observation to be added.
    /// \param bin2 The second bin index of the observation to be added.
    inline void add_observation(unsigned int bin1, unsigned int bin2, unsigned int index) {
        assert(index < histograms.size());

        histograms[index]->add_observation(bin1, bin2);
        sum_N(bin1, bin2)++;
        sum_n++;
    }

    /// Function for adding multidimensional observations to the histogram.
    ///
    /// \param bin The multidimensional index of the bin for the observation.
    inline void add_observation(std::vector<unsigned int> &bin) {
        assert(index < histograms.size());
        histograms[index]->add_observation(bin);
        sum_N(bin)++;
        sum_n++;
    }

    /// Function for the sum of counts in each bin. The returned array has the
    /// same shape as the individual histograms, and the \f$ i \f$'th bin of
    /// the sum histogram \f$ \mathrm{sum\_n} \f$ is
    ///
    /// \f[
    ///  \mathrm{sum\_n}[i] = \sum_j N_j[i]
    /// \f]
    ///
    /// where \f$ N_j \f$ is the \f$ j \f$'th histogram.
    ///
    /// \return The sum histogram.
    inline const CArray& get_sum_N() const {return sum_N;}

    /// Function for getting the total number of observation in the collection.
    /// That is the sum of counts over all bins in all histograms.
    ///
    /// \return The total number of observations in the collection.
    inline Count get_sum_n() const {return sum_n;}

    /// Get the number of Histograms in the collection.
    ///
    /// \return The number of histograms in the collection.
    inline unsigned int get_size() const {return histograms.size();}

    /// This function overloads the []-operator and gives access to the
    /// individual histograms. The function is safe in the sense, that it uses
    /// the deque function std::deque::at, which throws a out_of_range
    /// exception, if the the index is out of range.
    ///
    /// \param i The index of the histogram to access.
    /// \return The i'th histogram.
    inline const Histogram& operator[](unsigned int i) const {return *histograms.at(i);}

    /// Type of constant forward iterator for the HistogramCollection.
    typedef std::vector<Histogram*>::const_iterator const_iterator;

    /// \return Constant forward iterator pointing to the first element in the collection.
    inline const_iterator begin() const {return histograms.begin();}

    /// \return Constant forward iterator pointing one past the last element in the collection.
    inline const_iterator end() const {return histograms.end();}

private:
    std::vector<*Histogram> histograms;     ///< The list of histograms in the collection
    std::vector<unsigned int> shape;        ///< The shape of the histograms.
    CArray sum_N;                           ///< The sum of counts in each bin across all histograms.
    Count sum_n;                            ///< The total number of observations in the collection.
};

} /* namespace Muninn */
#endif /* MUNINN_HISTOGRAMCOLLECTION_H_ */

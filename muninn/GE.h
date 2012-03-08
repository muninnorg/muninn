// GE.h
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

#ifndef MUNINN_GE_H_
#define MUNINN_GE_H_

#include <cassert>
#include <deque>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/TArray.h"
#include "muninn/Histogram.h"
#include "muninn/History.h"
#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/Estimate.h"
#include "muninn/Estimator.h"
#include "muninn/UpdateScheme.h"
#include "muninn/WeightScheme.h"
#include "muninn/Binner.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

/// The class definition for the discrete generalized ensemble. In this class
/// all communication is done in terms of bin indices.class supports,
///
/// The interface of the GE class supports multidimensional ensembles, however
/// not all weight schemes supports more than one dimension.
///
/// Note that this class is rarely used directly by users, as the normal way to
/// interface with Muninn is through the CGE class.
class GE {
public:

    /// One dimensional constructor for the GE class.
    ///
    /// \param nbins The (initial) number of bins (0 is also allowed).
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the WeightScheme to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param receives_ownership Whether the CE class takes ownership of the
    ///                           Estimator, UpdateScheme, WeighScheme and
    ///                           StatisticsLogger objects. If set to true,
    ///                           these will be delete when the GE object is
    ///                           deleted.
    GE(unsigned int nbins, Estimator *estimator, UpdateScheme *updatescheme, WeightScheme *weightscheme, StatisticsLogger *statisticslogger=NULL, bool receives_ownership=false) :
        current(newvector(nbins)),
        history(estimator->new_history(newvector(nbins))),
        estimate(estimator->new_estimate(newvector(nbins))),
        estimator(estimator),
        updatescheme(updatescheme),
        weightscheme(weightscheme),
        statisticslogger(statisticslogger),
        has_ownership(receives_ownership),
        has_ownership_history(true) {
        init();
    }

    /// Two dimensional constructor for the GE class.
    ///
    /// \param nbins1 The (initial) number of bins in the first dimension (0
    ///               is also allowed).
    /// \param nbins2 The (initial) number of bins in the second dimension (0
    ///               is also allowed).
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the WeightScheme to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param receives_ownership Whether the CE class takes ownership of the
    ///                           Estimator, UpdateScheme, WeighScheme and
    ///                           StatisticsLogger objects. If set to true,
    ///                           these will be delete when the GE object is
    ///                           deleted.
    GE(unsigned int nbins1, unsigned int nbins2, Estimator *estimator, UpdateScheme *updatescheme, WeightScheme *weightscheme, StatisticsLogger *statisticslogger=NULL, bool receives_ownership=false) :
        current(newvector(nbins1, nbins2)),
        history(estimator->new_history(newvector(nbins1, nbins2))),
        estimate(estimator->new_estimate(newvector(nbins1, nbins2))),
        estimator(estimator),
        updatescheme(updatescheme),
        weightscheme(weightscheme),
        statisticslogger(statisticslogger),
        has_ownership(receives_ownership),
        has_ownership_history(true) {
        init();
    }

    /// Multidimensional constructor for the GE class.
    ///
    /// \param shape The (initial) shape of the histogram.
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the WeightScheme to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param receives_ownership Whether the CE class takes ownership of the
    ///                           Estimator, UpdateScheme, WeighScheme and
    ///                           StatisticsLogger objects. If set to true,
    ///                           these will be delete when the GE object is
    ///                           deleted.
    GE(const std::vector<unsigned int> &shape, Estimator *estimator, UpdateScheme *updatescheme, WeightScheme *weightscheme, StatisticsLogger *statisticslogger=NULL, bool receives_ownership=false) :
        current(shape),
        history(estimator->new_history(shape)),
        estimate(estimator->new_estimate(shape)),
        estimator(estimator),
        updatescheme(updatescheme),
        weightscheme(weightscheme),
        statisticslogger(statisticslogger),
        has_ownership(receives_ownership),
        has_ownership_history(true) {
        init();
    }

    /// Construct a GE object based on a given history. The shape of the
    /// current histogram is set based on the shape of the history.
    ///
    /// \param lnG The lnG used to construct the current estimate with. This is
    ///            also used for setting the weights.
    /// \param lnG_support The support of the estimated lnG.
    /// \param history The initial history.
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the WeightScheme to be used.
    /// \param binner A Binner should be passed if the binning of the entropy
    ///               and histogram is non-uniform.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param receives_ownership Whether the CE class takes ownership of the
    ///                           Estimator, UpdateScheme, WeighScheme,
    ///                           StatisticsLogger and History objects. If set
    ///                           to true, these will be delete when the GE
    ///                           object is deleted.
    GE(const DArray &lnG, const BArray &lnG_support, History *history, Estimator *estimator, UpdateScheme *updatescheme, WeightScheme *weightscheme,  const Binner *binner=NULL, StatisticsLogger *statisticslogger=NULL, bool receives_ownership=false) :
        current(newvector<unsigned int>(0)),
        history(history),
        estimate(estimator->new_estimate(lnG, lnG_support, *history, binner)),
        estimator(estimator),
        updatescheme(updatescheme),
        weightscheme(weightscheme),
        statisticslogger(statisticslogger),
        has_ownership(receives_ownership),
        has_ownership_history(receives_ownership) {
        init();

        // Set the weights correctly
        DArray new_weights = weightscheme->get_weights(*estimate, *history, binner);
        current = Histogram(new_weights); // TODO: Is this the best way of making an empty histogram?
    }

    /// Destructor for the GE class. If has_ownership is set to true, the
    /// Estimator, UpdateScheme, WeightScheme and StatisticsLogger objects are
    /// also deleted.
    virtual ~GE() {
        if (has_ownership_history) {
        	delete history;
        }

        delete estimate;

        if (has_ownership) {
            delete estimator;
            delete updatescheme;
            delete weightscheme;
            delete statisticslogger;
        }
    }

    /// Function for adding a one dimensional observation.
    ///
    /// \param bin The bin index of the observation.
    /// \return Returns true if new weights should be estimated.
    inline bool add_observation(unsigned int bin) {
        current.add_observation(bin);
        new_weights_variable = updatescheme->update_required(current, *history);
        return new_weights_variable;
    }

    /// Function for adding a two dimensional observation.
    ///
    /// \param bin1 The first bin index of the observation to be added.
    /// \param bin2 The second bin index of the observation to be added.
    /// \return Returns true if new weights should be estimated.
    inline bool add_observation(unsigned int bin1, unsigned int bin2) {
        current.add_observation(bin1, bin2);
        new_weights_variable = updatescheme->update_required(current, *history);
        return new_weights_variable;
    }

    /// Method for adding a multidimensional observation.
    ///
    /// \param bin The multidimensional index of the bin for the observation
    ///            to be added.
    /// \return Returns true if new weights should be estimated.
    inline bool add_observation(std::vector<unsigned int> &bin) {
        current.add_observation(bin);
        new_weights_variable = updatescheme->update_required(current, *history);
        return new_weights_variable;
    }

    /// Get the weigh associated with a bin, using a one dimensional index.
    ///
    /// \param bin The bin index.
    /// \return Returns the log weigh associated with the bin.
    inline double get_lnweights(int bin) {
        return current.get_lnw()(bin);
    }
    /// Get the weigh associated with a bin, using a two dimensional index.
    ///
    /// \param bin1 The first dimension of the bin index.
    /// \param bin2 The second dimension of the bin index.
    /// \return Returns the log weigh associated with the bin.
    inline double get_lnweights(int bin1, int bin2) {
        return current.get_lnw()(bin1, bin2);
    }

    /// Get the weigh associated with a bin, using a multidimensional index.
    ///
    /// \param bin The multidimensional bin index.
    /// \return Returns the log weigh associated with the bin.
    inline double get_lnweights(std::vector<unsigned int> &bin) {
        return current.get_lnw()(bin);
    }

    /// Method for determine if new weight should be estimated. Note that the
    /// method returns a cached values, and accordingly it is cheap to call.
    ///
    /// \return Returns true if new weights should be estimated.
    inline bool new_weights() {
        return new_weights_variable;
    }

    /// Function for estimating new weights. This function should be called
    /// when the function new_weights returns true. A binner should be passed
    /// to the function, if the binning is non-uniform.
    ///
    /// \param binner A Binner should be passed if the binning of the histogram
    ///               is non-uniform.
    void estimate_new_weights(const Binner *binner=NULL);

    /// Force the GE class to write stastics to the log (using the
    /// StatisticsLogger). If a Binner is passed, the state of the binner
    /// is also logged.
    ///
    /// \param binner If a Binner is passed, the state of the binner is also
    ///               logged. This should normaly be done, if the binning of
    ///               the histogram is non-uniform.
    inline void force_statistics_log(const Binner *binner=NULL) {
        if (statisticslogger)
            statisticslogger->log(current, *history, *estimate, binner);
    }

    /// Function for extending the shape of the GE object.
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    /// \param binner A Binner should be passed if the binning of the histogram
    ///               is non-uniform.
    void extend(const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over, const Binner *binner=NULL);

    /// Getter for the latest estimate of the entropy.
    ///
    /// \return A constant reference to the latest estimate of the entropy.
    inline const Estimate& get_estimate() const {return *estimate;}

    /// Getter for the current History.
    ///
    /// \return A constant reference to the current history.
    inline const History& get_history() const {return *history;}

    /// Getter for the current Histogram.
    ///
    /// \return A constant reference to the current histogram.
    inline const Histogram& get_current_histogram() const {return current;}

    /// Getter for the Estimator used by the GE object.
    ///
    /// \return A constant reference to the Estimator object.
    inline const Estimator& get_estimator() const {return *estimator;}

    /// Getter for the UpdateScheme used by the GE object.
    ///
    /// \return A constant reference to the UpdateScheme object.
    inline const UpdateScheme& get_updatescheme() const {return *updatescheme;}

    /// Getter for the WeightScheme used by the GE object.
    ///
    /// \return A constant reference to the WeightScheme object.
    inline const WeightScheme& get_weightscheme() const {return *weightscheme;}

    /// This functions tries to cast the current History object to the type
    /// MultiHistogramHistory and returns a reference to it. If the cast fails
    /// the function will throw a CastException.
    ///
    /// \return A constant reference to the History casted to a MultiHistogramHistory.
    inline const MultiHistogramHistory& get_multi_histogram_history() const {
        return *MultiHistogramHistory::cast_from_base(history, "Failed to cast the history in GE class as a MultiHistogramHistory.");
    }

    // Define friends
    friend class CGE;

private:
    Histogram current;                  ///< The current histogram.
    History *history;                   ///< The history of histograms.
    Estimate *estimate;                 ///< The current estimate of the density of states.

    Estimator *estimator;               ///< The Estimator in use.
    UpdateScheme *updatescheme;         ///< The UpdateScheme in use.
    WeightScheme *weightscheme;         ///< The WeightScheme in use.

    StatisticsLogger *statisticslogger;

    bool has_ownership;                 ///< Whether this GE object owns the Estimator, UpdateScheme, WeighScheme and StatisticsLogger objects.
    bool has_ownership_history;         ///< Whether this GE object owns the History object.

    int total_iterations;               ///< The total number of iterations recorded by the GE class, including observations dropped from the history.
    bool new_weights_variable;          ///< This variable is set to true, when the UpdateScheme says it is time to estimate new weights.

    /// Private method of initializing the class
    void init() {
        // Setup other variables
        total_iterations = 0;
        new_weights_variable = false;
    }
};

} // namespace Muninn

#endif // MUNINN_GE_H_

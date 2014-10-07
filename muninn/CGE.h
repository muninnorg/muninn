// CGE.h
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

#ifndef MUNINN_CGE_H_
#define MUNINN_CGE_H_

#include <cassert>
#include <deque>
#include <vector>

#include "muninn/utils/TArray.h"
#include "muninn/GE.h"
#include "muninn/Estimator.h"
#include "muninn/UpdateScheme.h"
#include "muninn/WeightScheme.h"
#include "muninn/ExtrapolatedWeightScheme.h"
#include "muninn/Binner.h"
#include "muninn/utils/StatisticsLogger.h"
#include "muninn/Exceptions/MaximalNumberOfBinsExceed.h"

namespace Muninn {

/// The Continuous Generalized Ensemble (CGE) class. The class constitutes
/// the main user interface to Muninn.
///
/// Note that while the GE class generally support any number of dimensions,
/// the CGE class currently only support one dimension.
class CGE {
public:

    /// Constructor based on pointers to the Estimator, UpdateScheme,
    /// WeighScheme and Binner objects. The class can take ownership of these
    /// objects and delete them upon destruction of the CGE object.
    ///
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the weightscheme to be used.
    /// \param binner A reference to the binner to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param initial_beta The beta to be used for collecting the first histogram.
    /// \param receives_ownership Whether the CGE class takes ownership of the
    ///                           Estimator, UpdateScheme and WeighScheme objects.
    ///                           If set to true, these will be delete when the
    ///                           CGE object is deleted.
    CGE(Estimator *estimator,
        UpdateScheme *updatescheme,
        WeightScheme *weightscheme,
        Binner *binner,
        StatisticsLogger *statisticslogger=NULL,
        double initial_beta=0,
        bool receives_ownership=false) :
            ge(0u, estimator, updatescheme, weightscheme, statisticslogger, receives_ownership),
            binner(binner),
            extrapolated_weightscheme(dynamic_cast<ExtrapolatedWeightScheme*>(weightscheme)),
            has_ownership(receives_ownership),
            initial_max(updatescheme->get_initial_max()),
            initial_collection(true),
            initial_beta(initial_beta) {
        add_loggables(statisticslogger);
    }

	/// Construct a CGE object based on a given estimate and a history. The
    /// shape of the history and the weights must match the number bins
    /// represented in the binner.
    ///
    /// The constructor is based on pointers to the Estimator, UpdateScheme,
    /// WeighScheme, Binner and History objects. The class can take ownership
    /// of these objects and delete them upon destruction of the CGE object.
    ///
    /// \param estimate The inital value of the estimate.
    /// \param history The initial history.
    /// \param estimator A pointer to the Estimator to be used.
    /// \param updatescheme A pointer to the UpdateScheme to be used.
    /// \param weightscheme A pointer to the weightscheme to be used.
    /// \param binner A reference to the binner to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param receives_ownership Whether the CGE class takes ownership of the
    ///                           Estimate, History, Estimator, UpdateScheme,
    ///                           and WeighScheme objects. If set to true,
    ///                           these will be delete when the CGE object is
    ///                           deleted.
    CGE(Estimate *estimate,
    	History *history,
    	Estimator *estimator,
        UpdateScheme *updatescheme,
        WeightScheme *weightscheme,
        Binner *binner,
        StatisticsLogger *statisticslogger=NULL,
        bool receives_ownership=false) :
            ge(estimate, history, estimator, updatescheme, weightscheme, binner, statisticslogger, receives_ownership),
            binner(binner),
            extrapolated_weightscheme(dynamic_cast<ExtrapolatedWeightScheme*>(weightscheme)),
            has_ownership(receives_ownership),
            initial_max(updatescheme->get_initial_max()),
            initial_collection(false),
            initial_beta(0.0) {

        // Check the shape of the binner
    	if(!(history->get_shape().size()==1 && history->get_shape()[0]==binner->get_nbins())) {
    		throw MessageException("The shape of the history given to the GE constructor must match the number of bins represented in the binner.");
    	}

    	add_loggables(statisticslogger);
    }

    /// Constructor based on reference of the Estimator, UpdateScheme and
    /// WeighScheme classes.
    ///
    /// \param estimator A reference to the Estimator to be used.
    /// \param updatescheme A reference to the UpdateScheme to be used.
    /// \param weightscheme A reference to the weightscheme to be used.
    /// \param binner A refernce to the binner to be used.
    /// \param statisticslogger A pointer to the StatisticsLogger (if set to
    ///                         null no statistics will be logged).
    /// \param initial_beta The beta to be used for collecting the first histogram.
    CGE(Estimator &estimator,
        UpdateScheme &updatescheme,
        WeightScheme &weightscheme,
        Binner &binner,
        StatisticsLogger *statisticslogger=NULL,
        double initial_beta=0) :
            ge(0u, &estimator, &updatescheme, &weightscheme, statisticslogger, false),
            binner(&binner),
            extrapolated_weightscheme(dynamic_cast<ExtrapolatedWeightScheme*>(&weightscheme)),
            has_ownership(false),
            initial_max(updatescheme.get_initial_max()),
            initial_collection(true),
            initial_beta(initial_beta) {
        add_loggables(statisticslogger);
    }

    /// Destructor for the CGE class. If has_ownership is set to true, the
    /// Estimator, UpdateScheme, WeightScheme, Binner and StatisticsLogger
    /// objects are automatically deleted. The CGE class is only responsible
    /// for deleting the Binner object, the remaining object are deleted by
    /// the GE class.
    virtual ~CGE() {
        if (has_ownership) {
            delete binner;
        }
    }

    /// Add a observation of an energy.
    ///
    /// \param energy The energy to be added.
    /// \return Returns true if new weights should be estimated.
    inline bool add_observation(double energy) {
        if (initial_collection) {
            initial_observations.push_back(energy);
            return initial_new_weights();
        }
        else {
            unsigned int bin;
            try {
                bin = calc_bin_with_extention(energy);
            }
            catch (MaximalNumberOfBinsExceed& exception) {
                MessageLogger::get().warning(exception.what());
                return ge.new_weights();
            }
            return ge.add_observation(bin);
        }
    }

    /// Get the log weighs for an energy.
    ///
    /// \param energy The energy to get a weigh for.
    /// \return The log weight for the energy.
    inline double get_lnweights(double energy) {
        // Use Boltzmann weights for the initial collection
        if (initial_collection) {
            return -initial_beta * energy;
        }
        // Look up the weights for the remaining
        else {
            // If the weightscheme is extrapolated, the weight outside the binning can be calculated without extension
            if (extrapolated_weightscheme) {
                // Calculate the bin
                std::pair<int, bool> bin = binner->calc_bin_validated(energy);

                // If we are outside the range, return the extrapolated weight
                if (!bin.second) {
                    return extrapolated_weightscheme->get_extrapolated_weight(energy, ge.get_current_histogram().get_lnw(), ge.get_estimate(), ge.get_history(), *binner);
                }
                // If we are inside the range, return the weight
                else {
                    return ge.get_lnweights(bin.first);
                }
            }
            // With a normal weightscheme, the binning has the be extended in order to get a weight
            else {
                unsigned int bin;
                try {
                    bin = calc_bin_with_extention(energy);
                }
                catch (MaximalNumberOfBinsExceed& exception) {
                    MessageLogger::get().warning(exception.what());
                    return std::numeric_limits<double>::infinity();
                }
                return ge.get_lnweights(bin);
            }
        }
    }

    /// Function for determining if it time to estimate new weights. Note that
    /// the method returns a cached values, and accordingly it is cheap to call.
    ///
    /// \return Returns true if new weights should be estimated.
    inline bool new_weights() {
        if (initial_collection)
            return initial_new_weights();
        else
            return ge.new_weights();
    }

    /// Function for estimating new weights. This function should be called
    /// when the function new_weights returns true.
    void estimate_new_weights();

    /// Force the current statistics to be logged with the logger.
    inline void force_statistics_log() {
        ge.force_statistics_log();
    }

    /// Const getter for the used binner
    ///
    /// \return The binner in use.
    inline const Binner & get_binner() const {return *binner;}

    /// Getter for the used binner
    ///
    /// \return The binner in use.
    inline Binner & get_binner() {return *binner;}

    /// Shortcut to get the bin edges from the Binner object.
    ///
    /// \return The current bin edges.
    inline DArray get_binning() const {
        return binner->get_binning();
    }

    /// Shortcut to get the current bin centers from the Binner object.
    ///
    /// \return The current bin centers
    inline DArray get_binning_centered() const {
        return binner->get_binning_centered();
    }

    /// Get the discrete GE objected used by the CGE object.
    ///
    /// \return The discrete GE objected used by the CGE object.
    inline const GE & get_ge() const {return ge;}

    // Define friends
    friend class CGEcollection;

private:
    // General variables
    GE ge;                                               ///< The discrete GE object used by this class.
    Binner *binner;                                      ///< The binner in use.
    ExtrapolatedWeightScheme *extrapolated_weightscheme; ///< If this is not null, the weight scheme implements the ExtrapolatedWeightScheme interface.
    bool has_ownership;                                  ///< Whether this CGE object owns the Estimator, UpdateScheme and WeighScheme objects.

    // Variables for the initial collection of data points
    unsigned int initial_max;                            ///< The number of energy observation to be collected for the initial histogram.
    bool initial_collection;                             ///< Whether energies currently are been collected for the initial histogram.
    std::vector<double> initial_observations;            ///< The collected initial observed energies.
    double initial_beta;                                 ///< The beta used in Boltzmann weights for the initial observations.

    // Private methods

    /// Method for determining if new weights should be estimated in the
    /// collection of observations for the first histogram.
    ///
    /// \return Returns true if new weight should be estimated.
    inline bool initial_new_weights() {
        return initial_observations.size() > initial_max;
    }

    /// Calculate the bin number corresponding to an energy,
    /// and extend the binned area if required.
    ///
    /// \param energy The energy to calculate the bin for.
    /// \return The bin number corresponding to the given energy.
    inline unsigned int calc_bin_with_extention(double energy) {
        std::pair<int, bool> bin = binner->calc_bin_validated(energy);

        // Check if we are outside the range of the bins and resize if we are
        if (!bin.second) {
            // Extend the binning and get the size of the extension
            std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension = binner->extend(energy, ge.get_estimate(), ge.get_history(), ge.get_current_histogram().get_lnw());

            // Extend the binner and the GE object
            ge.extend(extension.first, extension.second, binner);

            // Recalculate the bin value
            bin = binner->calc_bin_validated(energy);
        }
        return static_cast<unsigned int>(bin.first);
    }

    /// Private function adding loggable classes to the statisticslogger.
    ///
    /// \param statisticslogger A pointer to the StatisticsLogger (may be NULL).
    void add_loggables(StatisticsLogger *statisticslogger=NULL) {
        if (statisticslogger!=NULL) {
            statisticslogger->add_loggable(binner);
        }
    }
};

} // namespace Muninn

#endif // MUNINN_CGE_H_

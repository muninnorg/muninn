// CGEfactory.cpp
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

#include "muninn/Factories/CGEfactory.h"
#include "muninn/CGE.h"
#include "muninn/MLE/MLE.h"
#include "muninn/Binners/UniformBinner.h"
#include "muninn/Binners/NonUniformDynamicBinner.h"
#include "muninn/UpdateSchemes/IncreaseFactorScheme.h"
#include "muninn/WeightSchemes/LinearPolatedMulticanonical.h"
#include "muninn/WeightSchemes/LinearPolatedInvK.h"
#include "muninn/WeightSchemes/LinearPolatedInvKP.h"
#include "muninn/WeightSchemes/FixedWeights.h"

#include "muninn/utils/StatisticsLogReader.h"
#include "muninn/utils/ArrayAligner.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/utils/GenericEnumStreamOperators.h"


namespace Muninn {

CGE* CGEfactory::new_CGE(const Settings& settings) {
    // Set the Muninn debug level
    MessageLogger::get().set_verbose(settings.verbose);

    // Possibly read a statistics log file
    StatisticsLogReader *statistics_log_reader = NULL;
    std::string read_statistics_log_filename = "";
    StatisticsLogger::Mode read_statistics_log_mode = StatisticsLogger::NONE;

    if (settings.continue_statistics_log) {
        read_statistics_log_filename = settings.statistics_log_filename;
    }
    else if (settings.read_statistics_log_filename!="") {
        read_statistics_log_filename = settings.read_statistics_log_filename;
    }

    if (read_statistics_log_filename!="") {
        MessageLogger::get().info("Reading statistics log file");
    	statistics_log_reader = new StatisticsLogReader(read_statistics_log_filename, settings.memory);

    	unsigned int number_of_histograms = statistics_log_reader->get_Ns().size();

    	// Guess the mode that the history was written with
        if (statistics_log_reader->get_Ns().size()==number_of_histograms &&
            statistics_log_reader->get_lnws().size()==number_of_histograms &&
            statistics_log_reader->get_lnGs().size()==number_of_histograms &&
            statistics_log_reader->get_lnG_supports().size()==number_of_histograms &&
            statistics_log_reader->get_binnings().size()==number_of_histograms &&
            statistics_log_reader->get_free_energies().size()==number_of_histograms &&
            statistics_log_reader->get_this_maxs().size()==number_of_histograms &&
            statistics_log_reader->get_x_zeros().size()==number_of_histograms) {
            read_statistics_log_mode = StatisticsLogger::ALL;
        }
        else if (statistics_log_reader->get_Ns().size()==number_of_histograms &&
                statistics_log_reader->get_lnws().size()==number_of_histograms &&
                statistics_log_reader->get_lnGs().size()==1 &&
                statistics_log_reader->get_lnG_supports().size()==1 &&
                statistics_log_reader->get_binnings().size()==1 &&
                statistics_log_reader->get_free_energies().size()==1 &&
                statistics_log_reader->get_this_maxs().size()==1 &&
                statistics_log_reader->get_x_zeros().size()==1) {
            read_statistics_log_mode = StatisticsLogger::CURRENT;
        }
        else {
            throw(CGEfactorySettingsException("Error: The given log file does not contains adequate information."));
        }

        // Print info about the read history
        unsigned int number_of_free_energies = statistics_log_reader->get_free_energies().back().second.get_shape().at(0);

        MessageLogger::get().debug("Maximal number of histograms that can be read from log file: " + to_string(number_of_free_energies));
        MessageLogger::get().debug("History mode: " + to_string(read_statistics_log_mode));

        // Check that settings.memory matches the number of available free energies
        if (number_of_free_energies != settings.memory) {
            MessageLogger::get().warning("Memory (" + to_string(settings.memory) + ") differs from the number of free energies available (" + to_string(number_of_free_energies) + ")");
            MessageLogger::get().warning(to_string(number_of_free_energies) + " histograms will be used from log file");
        }
    }

    // Allocate the estimator
    Estimator *estimator = NULL;

    switch (settings.estimator) {
    case ESTIMATOR_MLE :
        estimator = new MLE(settings.min_count, settings.memory, settings.restricted_individual_support);
        break;
    default :
        throw(CGEfactorySettingsException("Estimator not set correctly."));
    }

    // Allocate the update scheme
    unsigned int initial_max;

    if (statistics_log_reader==NULL) {
    	initial_max = settings.initial_max;
    }
    else {
    	initial_max = statistics_log_reader->get_this_maxs().back().second(0);
    	MessageLogger::get().debug("Settings initial_max to "+to_string(initial_max)+".");
    }

    IncreaseFactorScheme* update_scheme = new IncreaseFactorScheme(initial_max, settings.increase_factor, settings.max_iterations_per_histogram);

    // Allocate the weight scheme
    LinearPolatedWeigths *weight_scheme = NULL;

    if (settings.read_fixed_weights_filename=="") {
        switch (settings.weight_scheme) {
        case GE_MULTICANONICAL :
            weight_scheme = new LinearPolatedMulticanonical(settings.slope_factor_up, settings.slope_factor_down);
            break;
        case GE_INV_K :
            weight_scheme = new LinearPolatedInvK(settings.slope_factor_up, settings.slope_factor_down);
            break;
        case GE_INV_K_P :
            weight_scheme = new LinearPolatedInvKP(settings.slope_factor_up, settings.slope_factor_down, settings.p);
            break;
        default:
            throw(CGEfactorySettingsException("Weight Scheme not set correctly."));
        }
    }
    else {
        // Read the file containing the fixed weights
        std::ifstream input(settings.read_fixed_weights_filename.c_str());
        std::string line;
        std::getline(input, line);
        double reference_value = from_string<double>(line);
        std::getline(input, line);
        DArray fixed_weights = from_string<DArray>(line);
        input.close();

        // Check that an array with weight was read
        if (!fixed_weights.nonempty()) {
            throw(CGEfactorySettingsException("Could not read weights from file with fixed weights."));
        }

        // Setup the inner weight scheme
        WeightScheme *inner_weight_scheme = NULL;

        if (settings.weight_scheme == GE_MULTICANONICAL)
            inner_weight_scheme = new Multicanonical;
        else if (settings.weight_scheme == GE_INV_K)
            inner_weight_scheme = new InvK;
        else if (settings.weight_scheme == GE_INV_K_P)
            inner_weight_scheme = new InvKP(settings.p);
        else {
            throw(CGEfactorySettingsException("Weight Scheme not set correctly."));
        }

        // Setup the fixed weights
        WeightScheme *fixed_weight_scheme = new FixedWeights(reference_value, fixed_weights, inner_weight_scheme);

        // Setup the linear polated weight scheme
        weight_scheme = new LinearPolatedWeigths(fixed_weight_scheme, settings.slope_factor_up, settings.slope_factor_down, 20, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), true);
    }

    // Setup the minimum beta and maximum beta
    weight_scheme->set_min_beta_extrapolation(settings.min_beta);
    weight_scheme->set_min_beta_thermodynamics(settings.min_beta);
    weight_scheme->set_max_beta_thermodynamics(settings.max_beta);

    // Allocate the binner
    Binner* binner = NULL;

    if (settings.use_dynamic_binning) {
        if (statistics_log_reader==NULL) {
        	binner = new NonUniformDynamicBinner(settings.resolution, settings.initial_width_is_max_left, settings.initial_width_is_max_right, settings.max_number_of_bins);
        }
        else {
        	binner = new NonUniformDynamicBinner(statistics_log_reader->get_binnings().back().second, settings.initial_beta, settings.resolution, settings.initial_width_is_max_left, settings.initial_width_is_max_right, settings.max_number_of_bins);
        }
    }
    else {
        if (statistics_log_reader==NULL) {
        	// TODO: This constructor should also use the max_number_of_bins
        	binner = new UniformBinner(settings.bin_width);
        }
        else {
        	binner = new UniformBinner(statistics_log_reader->get_binnings().back().second.min(), statistics_log_reader->get_binnings().back().second.max(), statistics_log_reader->get_binnings().back().second.get_asize()-1);
        }
    }

    // Allocate the logger
    StatisticsLogger* statistics_logger;

    if (settings.continue_statistics_log) {
        unsigned int counter_offset = 0;
        if (statistics_log_reader->get_Ns().size()>0) {
            counter_offset = from_string<unsigned int>(statistics_log_reader->get_Ns().back().first.substr(1)) + 1;
        }
        statistics_logger = new StatisticsLogger(settings.statistics_log_filename, settings.log_mode, settings.log_precision, true, counter_offset);
    }
    else {
        statistics_logger = new StatisticsLogger(settings.statistics_log_filename, settings.log_mode, settings.log_precision);
    }

    // Create the CGE object
    CGE* cge = NULL;

    if (statistics_log_reader==NULL) {
    	cge = new CGE(estimator, update_scheme, weight_scheme, binner, statistics_logger, settings.initial_beta, true);
    }
    else {
        unsigned int number_of_free_energies = statistics_log_reader->get_free_energies().back().second.get_shape().at(0);

        // Reconstruct the history
        History* history = estimator->new_history(statistics_log_reader->get_Ns().back().second.get_shape());

    	for (unsigned int i = 0; i < Muninn::min<unsigned int>(statistics_log_reader->get_Ns().size(), number_of_free_energies); ++i) {
    	    if (read_statistics_log_mode == StatisticsLogger::ALL) {
    	        // Compare the binning of the curren and the final histogram
                const DArray& current_binning = statistics_log_reader->get_binnings().at(i).second;
                const DArray& final_binning = statistics_log_reader->get_binnings().back().second;

                // Check if the binning is the same as the last histogram
                if (!current_binning.same_shape(final_binning)) {
                    std::pair<unsigned int, unsigned int> offsets = ArrayAligner::calculate_alignment_offsets(final_binning, current_binning);
                    const CArray extended_Ns = statistics_log_reader->get_Ns().at(i).second.extended(offsets.first, offsets.second);
                    const DArray extended_lnws = statistics_log_reader->get_lnws().at(i).second.extended(offsets.first, offsets.second);
                    history->add_histogram(new Histogram(extended_Ns, extended_lnws));
                }
                else {
                    history->add_histogram(new Histogram(statistics_log_reader->get_Ns().at(i).second, statistics_log_reader->get_lnws().at(i).second));
                }
    	    }
    	    else {
    	        history->add_histogram(new Histogram(statistics_log_reader->get_Ns().at(i).second, statistics_log_reader->get_lnws().at(i).second));
    	    }
    	}

    	// Construct a new estimate
    	MLE* mle_estimator = MLE::cast_from_base(estimator, "The history can only be loaded for the MLE estimator.");

    	Estimate* estimate = mle_estimator->new_estimate(statistics_log_reader->get_lnGs().back().second,
    	                                                 statistics_log_reader->get_lnG_supports().back().second,
                                                         TArray_to_vector<Index>(statistics_log_reader->get_x_zeros().back().second),
                                                         statistics_log_reader->get_free_energies().back().second,
                                                         *history, binner, true);

    	// Construct the CGE object
    	cge = new CGE(estimate, history, estimator, update_scheme, weight_scheme, binner, statistics_logger, true);
    }

    return cge;
}

// Input GeEnum from string
std::istream &operator>>(std::istream &input, GeEnum &g) {
    return input_operator<GeEnum>(input, g, GE_ENUM_SIZE, GeEnumNames);
}

// Output GeEnum
std::ostream &operator<<(std::ostream &output, const GeEnum &g) {
    return output_operator<GeEnum>(output, g, GE_ENUM_SIZE, GeEnumNames);
}

// Input EstimatorEnum from string
std::istream &operator>>(std::istream &input, EstimatorEnum &e) {
    return input_operator<EstimatorEnum>(input, e, ESTIMATOR_ENUM_SIZE, EstimatorEnumNames);
}

// Output EstimatorEnum
std::ostream &operator<<(std::ostream &output, const EstimatorEnum &e) {
    return output_operator<EstimatorEnum>(output, e, ESTIMATOR_ENUM_SIZE, EstimatorEnumNames);
}

/// Input operator of a StatisticsLogger::Mode from string.
std::istream &operator>>(std::istream &input, StatisticsLogger::Mode &m) {
    return input_operator<StatisticsLogger::Mode>(input, m, StatisticsLogger::SIZE, StatisticsLogger::ModeNames);
}

/// Output operator for a StatisticsLogger::Mode
std::ostream &operator<<(std::ostream &putput, const StatisticsLogger::Mode &m) {
    return output_operator<StatisticsLogger::Mode>(putput, m, StatisticsLogger::SIZE, StatisticsLogger::ModeNames);
}

} // namespace Muninn

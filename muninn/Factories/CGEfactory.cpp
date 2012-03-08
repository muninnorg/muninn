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

#include "muninn/utils/StatisticsLogReader.h"
#include "muninn/utils/ArrayAligner.h"

namespace Muninn {

CGE* CGEfactory::new_CGE(const Settings& settings) {
    // Set the Muninn debug level
    MessageLogger::get().set_verbose(settings.verbose);

    // Possibly read a statistics log file
    StatisticsLogReader *statistics_log_reader = NULL;

    if (settings.read_statistics_log_filename!="") {
        MessageLogger::get().info("Reading statistics log file");
    	statistics_log_reader = new StatisticsLogReader(settings.read_statistics_log_filename, settings.memory);
    }

    // Allocate the estimator
    MLE* estimator = new MLE(settings.min_count, settings.memory, settings.restricted_individual_support);

    // Allocate the update scheme
    unsigned int initial_max;

    if (statistics_log_reader==NULL) {
    	initial_max = settings.initial_max;
    }
    else {
    	initial_max = statistics_log_reader->get_Ns().back().second.sum();
    	MessageLogger::get().debug("Settings initial_max to "+to_string(initial_max)+".");
    }

    IncreaseFactorScheme* update_scheme = new IncreaseFactorScheme(initial_max);

    // Allocate the weight scheme
    LinearPolatedWeigths *weight_scheme = NULL;

    if (settings.weight_scheme == GE_MULTICANONICAL)
        weight_scheme = new LinearPolatedMulticanonical(settings.slope_factor_up, settings.slope_factor_down);
    else if (settings.weight_scheme == GE_INV_K)
        weight_scheme = new LinearPolatedInvK(settings.slope_factor_up, settings.slope_factor_down);
    else {
        throw(CGEfactorySettingsException("Weight Scheme not set correctly."));
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
    StatisticsLogger* statistics_logger = new StatisticsLogger(settings.statistics_log_filename, settings.log_mode);

    // Create the CGE object
    CGE* cge = NULL;

    if (statistics_log_reader==NULL) {
    	cge = new CGE(estimator, update_scheme, weight_scheme, binner, statistics_logger, settings.initial_beta, true);
    }
    else {
    	History* history = estimator->new_history(statistics_log_reader->get_Ns().back().second.get_shape());

    	for (unsigned int i = 0; i < statistics_log_reader->get_Ns().size(); ++i) {
    		const DArray& current_binning = statistics_log_reader->get_binnings()[i].second;
    		const DArray& final_binning = statistics_log_reader->get_binnings().back().second;

    		// Check if the binning is the same as the last histogram
    		if (!current_binning.same_shape(final_binning)) {
    			std::pair<unsigned int, unsigned int> offsets = ArrayAligner::calculate_alignment_offsets(final_binning, current_binning);
        		const CArray extended_Ns = statistics_log_reader->get_Ns()[i].second.extended(offsets.first, offsets.second);
        		const DArray extended_lnws = statistics_log_reader->get_lnws()[i].second.extended(offsets.first, offsets.second);
        		history->add_histogram(Histogram(extended_Ns, extended_lnws));
    		}
    		else {
        		history->add_histogram(Histogram(statistics_log_reader->get_Ns()[i].second, statistics_log_reader->get_lnws()[i].second));
    		}
    	}

    	cge = new CGE(statistics_log_reader->get_lnGs().back().second, statistics_log_reader->get_lnG_supports().back().second, history, estimator, update_scheme, weight_scheme, binner, statistics_logger, true);
    }

    return cge;
}

// Input GeEnum from string
std::istream &operator>>(std::istream &input, GeEnum &g) {
     std::string raw_string;
     input >> raw_string;

     for (unsigned int i=0; i<GE_ENUM_SIZE; ++i) {
          if (raw_string == GeEnumNames[i]) {
               g = GeEnum(i);
          }
     }
     return input;
}

// Output GeEnum
std::ostream &operator<<(std::ostream &o, const GeEnum &g) {
     o << GeEnumNames[static_cast<unsigned int>(g)];
     return o;
}

} // namespace Muninn

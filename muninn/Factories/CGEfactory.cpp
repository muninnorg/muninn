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

namespace Muninn {

CGE* CGEfactory::new_CGE(const Settings& settings) {
    // Set the Muninn debug level
    MessageLogger::get().set_verbose(settings.verbose);

    // Allocate the estimator and the update scheme
    MLE* estimator = new MLE(settings.min_count, settings.memory, settings.restricted_individual_support);
    IncreaseFactorScheme* update_scheme = new IncreaseFactorScheme(settings.initial_max);

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

    if (settings.use_dynamic_binning)
        binner = new NonUniformDynamicBinner(settings.resolution, settings.initial_width_is_max_left, settings.initial_width_is_max_right);
    else
        binner = new UniformBinner(settings.bin_width);

    // Allocate the looger
    StatisticsLogger* statistics_logger = new StatisticsLogger(settings.statistics_log_filename, settings.log_mode);

    // Create the CGE object
    CGE* cge = new CGE(estimator, update_scheme, weight_scheme, binner, statistics_logger, settings.initial_beta, true);

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

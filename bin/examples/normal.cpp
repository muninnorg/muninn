// normal.cpp
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

#include <iostream>
#include <ctime>
#include <cstdlib>

#include "muninn/Factories/CGEfactory.h"

#include "NormalSampler.h"
#include "../details/OptionParser.h"

/// Function for getting a random number between 0 and 1
///
/// \return A random number between 0 and 1.
double inline drandom() {
    return 1.0 * rand() / RAND_MAX;
}

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("An example of using Muninn to sample from a normal distribution.");
    parser.add_option("-l", "statistics_log", "The Muninn statics log file", "muninn.txt");
    parser.add_option("-L", "log_mode", "The mode for the logger (options are ALL or CURRENT)", "all");
    parser.add_option("-s", "mcmc_steps", "Number of MCMC steps", "1E7");
    parser.add_option("-S", "seed", "The seed for the normal sampler, by default the time is used");
    parser.add_option("-r", "read_statistics_log", "Read a Muninn statics log file", "");
    parser.add_option("-R", "restart", "Enable restarts", "0", "1");
    parser.parse_args(argc, argv);

    // Set the random seed
    unsigned int seed;
    if (parser.has("seed")) {
        seed = parser.get_as<unsigned int>("seed");
    }
    else {
        seed = time(NULL);
    }
    srand(seed);

    // Set number of MCMC steps
    const unsigned long long int mcmc_steps = static_cast<unsigned long long int>(parser.get_as<double>("mcmc_steps"));

    // Set the restart options
    bool restart = parser.get_as<bool>("restart");

    // Print settings
    std::cout << "Seed: " << seed << std::endl;
    std::cout << "MCMC steps: " << mcmc_steps << std::endl;
    std::cout << "Restart: " << restart << std::endl;

    // Setup the MCMC
    Muninn::CGEfactory::Settings settings;

    settings.weight_scheme = Muninn::GE_MULTICANONICAL;
    settings.estimator = Muninn::ESTIMATOR_MLE;
    settings.initial_width_is_max_right = true;
    settings.statistics_log_filename = parser.get("statistics_log");
    settings.log_mode = parser.get_as<Muninn::StatisticsLogger::Mode>("log_mode");
    settings.read_statistics_log_filename = parser.get("read_statistics_log");
    settings.verbose = 3;

    std::cout << settings;

    Muninn::CGE *cge = Muninn::CGEfactory::new_CGE(settings);

    // Sample the initial candidate
    NormalSampler ns(0, 1, 10);

    double curr_state = ns.energy();
    double next_state;

    int updatecounter = 0;

    // The MCMC loop
    for (unsigned long long int step=0; step<mcmc_steps; ++step) {
        if (step % 10000 == 0)
            printf("\n########## MCMC STEP %llu (%.1f%%) ##########\n\n", step, 100.0*step/mcmc_steps);

        // Sample next
        ns.sample();
        next_state = ns.energy();

        // See if we should accept or reject candidate
        if (drandom() < exp(cge->get_lnweights(next_state) - cge->get_lnweights(curr_state)) + ns.log_bias()) {
            curr_state = next_state;
        } else {
            ns.step_one_back();
        }

        // Add the observation to the GE object
        cge->add_observation(curr_state);

        // See if it's time for new weights
        if (cge->new_weights()) {
            updatecounter++;

            // Calculate new estimate of the free energy surface
            cge->estimate_new_weights();

            // Restart every 4th time
            if (restart && updatecounter % 4 == 0) {
                ns.sample_first();
                curr_state = ns.energy();
            }
        }
    }

    // Clean up
    delete cge;

    return EXIT_SUCCESS;
}

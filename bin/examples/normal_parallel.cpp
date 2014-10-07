// normal_parallel.cpp
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

#include "muninn/Factories/CGEfactory.h"
#include "muninn/CGEcollection.h"

#include "NormalSampler.h"

#include "details/random_utils.h"
#include "../details/OptionParser.h"

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("An example of using Muninn to sample from a normal distribution with virtual parallel threads.");
    parser.add_option("-l", "statistics_log", "The Muninn statics log file", "muninn_%id%.txt");
    parser.add_option("-L", "log_mode", "The mode for the logger (options are ALL or CURRENT)", "all");
    parser.add_option("-s", "mcmc_steps", "Number of MCMC steps", "1E7");
    parser.add_option("-S", "seed", "The seed for the normal sampler, by default the time is used");
    parser.add_option("-r", "read_statistics_log", "Read a Muninn statics log file", "");
    parser.add_option("-t", "nthreads", "Number of virtual threads", "1");
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

    // Set the number of threads option
    unsigned int nthreads = parser.get_as<unsigned int>("nthreads");

    // Print settings
    std::cout << "Seed: " << seed << std::endl;
    std::cout << "MCMC steps: " << mcmc_steps << std::endl;

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

    Muninn::CGEcollection cge_collection;
    std::vector<NormalSampler> nss;


    for (unsigned int i=0; i<nthreads; ++i) {
        settings.statistics_log_filename = parser.get("statistics_log");
        size_t pos = settings.statistics_log_filename.find("%id%");
        settings.statistics_log_filename.replace(pos, 4, Muninn::to_string(i));
        Muninn::CGE *cge = Muninn::CGEfactory::new_CGE(settings);
        cge_collection.add_cge(cge, true);

        nss.push_back(NormalSampler(0, 1, 10));
    }

    // The MCMC loop
    for (unsigned long long int step=0; step<mcmc_steps; ++step) {
        if (step % 10000 == 0)
            printf("\n########## MCMC STEP %llu (%.1f%%) ##########\n\n", step, 100.0*step/mcmc_steps);

        for (unsigned int thread_id=0; thread_id<nthreads; ++thread_id) {
            // Sample next
            double curr_state = nss.at(thread_id).energy();
            nss.at(thread_id).sample();
            double next_state = nss.at(thread_id).energy();

            // See if we should accept or reject candidate
            if (randomD() < exp(cge_collection.at(thread_id).get_lnweights(next_state) - cge_collection.at(thread_id).get_lnweights(curr_state)) + nss.at(thread_id).log_bias()) {
                curr_state = next_state;
            } else {
                nss.at(thread_id).step_one_back();
            }

            // Add the observation to the GE object
            cge_collection.at(thread_id).add_observation(curr_state);
        }

        // See if it's time for new weights
        if (cge_collection.at(0).new_weights()) {
            cge_collection.unify_binners_range();
            for (unsigned int i=0; i<nthreads; ++i) {
                cge_collection.at(i).estimate_new_weights();
                std::cout << cge_collection.at(i).new_weights() << std::endl;
            }
            cge_collection.unify_binners_extension();

            assert(cge_collection.check_consistent_binning());
        }
    }

    return EXIT_SUCCESS;
}

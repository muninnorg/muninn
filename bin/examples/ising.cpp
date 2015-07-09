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

#include "muninn/Factories/CGEfactory.h"

#include "Ising2dSampler.h"

#include "details/random_utils.h"
#include "../details/OptionParser.h"

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("An example of using Muninn to simulate from the Ising model.");
    parser.add_option("-N", "ising_size", "The size of the side of the Ising system", "48");
    parser.add_option("-s", "mcmc_steps", "Number of MCMC steps", "1E7");
    parser.add_option("-S", "seed", "The seed for the Ising model and in the acceptance criteria, by default the time is used");
    parser.add_option("-W", "weight_scheme", "The Muninn weight scheme (multicanonical|invk)", "multicanonical");
    parser.add_option("-E", "estimator", "The Muninn estimator (MLE)", "MLE");
    parser.add_option("-w", "bin_width", "The Muninn bin width", "4.0");
    parser.add_option("-l", "statistics_log", "The Muninn statistics log file", "muninn.txt");
    parser.add_option("-L", "log_mode", "The mode for the logger (options are ALL or CURRENT)", "all");
    parser.add_option("-r", "read_statistics_log", "Read a Muninn statics log file", "");

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

    // Set number of MCMC steps and Ising system size
    const unsigned long long int mcmc_steps = static_cast<unsigned long long int>(parser.get_as<double>("mcmc_steps"));
    const unsigned int ising_size = parser.get_as<unsigned int>("ising_size");

    // Print settings
    std::cout << "Ising size: " << ising_size << std::endl;
    std::cout << "MCMC steps: " << mcmc_steps << std::endl;
    std::cout << "Seed: " << seed << std::endl;

    // Setup the MCMC
    Muninn::CGEfactory::Settings settings;
    settings.weight_scheme = parser.get_as<Muninn::GeEnum>("weight_scheme");
    settings.estimator = parser.get_as<Muninn::EstimatorEnum>("estimator");
    settings.binner = Muninn::UNIFORM;
    settings.bin_width = parser.get_as<double>("bin_width");
    settings.statistics_log_filename = parser.get("statistics_log");
    settings.log_mode = parser.get_as<Muninn::StatisticsLogger::Mode>("log_mode");
    settings.read_statistics_log_filename = parser.get("read_statistics_log");
    settings.verbose = 3;

    std::cout << settings;

    Muninn::CGE *cge = Muninn::CGEfactory::new_CGE(settings);

    // Setup the sampler
	Ising2dSampler<int> ising_sampler(ising_size);

	double curr_energy = ising_sampler.get_E();

    // The MCMC loop
	for (unsigned long long int step=0; step<mcmc_steps; ++step) {
        if (step % 10000 == 0)
            printf("\n########## MCMC STEP %llu (%.1f%%) ##########\n\n", step, 100.0*step/mcmc_steps);

        // Sample next
        ising_sampler.move();
        double next_energy = ising_sampler.get_E();

        // See if we should accept or reject candidate
        if (randomD() < exp(cge->get_lnweights(next_energy) - cge->get_lnweights(curr_energy))) {
            curr_energy = next_energy;
        } else {
        	ising_sampler.undo();
        }

        // Add the observation to the GE object
        cge->add_observation(curr_energy);

        // See if it's time for new weights
        if (cge->new_weights()) {
            // Calculate new estimate of the free energy surface
            cge->estimate_new_weights();
        }
    }

    // Clean up
    delete cge;

    return EXIT_SUCCESS;
}

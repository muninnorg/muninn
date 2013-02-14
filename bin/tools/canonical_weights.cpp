// canonical_weights.cpp
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

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "../details/OptionParser.h"
#include "muninn/tools/CanonicalAveragerFromStatisticsLog.h"

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("Calculates weights to be used in an canonical average at beta.");

    parser.add_option("-l", "statistics_log", "The Muninn statics log file (e.g. Muninn.txt)", OptionParser::REQUIRED);
    parser.add_option("-e", "energy_filename", "The file containing energies, separated by white spaces. Use -- for STDIN", "--");
    parser.add_option("-w", "weight_filename", "The file to output the weights corresponding to each energy, separated by white spaces. Use -- for STDOUT", "--");
    parser.add_option("-b", "beta", "The value of beta.", "1.0");
    parser.parse_args(argc, argv);

    // Make the CanonicalAverager
    Muninn::CanonicalAveragerFromStatisticsLog *ca=NULL;
    try {
        ca = new Muninn::CanonicalAveragerFromStatisticsLog(parser.get("statistics_log"));
    }
    catch (Muninn::MessageException& exception) {
        parser.parser_error(exception.what());
    }

    // Open stream for stdin
    std::istream *input;
    std::istream *input_file = NULL;

    if (parser.get("energy_filename") == "--") {
        input = &std::cin;
    }
    else {
        input_file = new std::ifstream(parser.get("energy_filename").c_str());
        input = input_file;
    }

    if (input->fail()) {
        parser.parser_error("Could not open input file.");
    }

    // Open file for stdout
    std::ostream *output;
    std::ostream *output_file = NULL;

    if (parser.get("weight_filename") == "--") {
        output = &std::cout;
    }
    else {
        output_file = new std::ofstream(parser.get("weight_filename").c_str());
        output = output_file;
    }

    if (output->fail()) {
        parser.parser_error("Could not open output file.");
    }

    // Read the energies for stdin
    std::vector<double> energies;
    std::map<unsigned int, std::string> white_spaces;

    while (input->good()) {
        // Check if the next char is a whitespace
        if (isspace(input->peek())) {
            std::string white_space;

            do {
                white_space += input->get();
            } while (isspace(input->peek()));

            white_spaces[energies.size()] = white_space;
        }
        // Read an energy
        else {
            double energy;
            (*input) >> energy;
            energies.push_back(energy);
        }
    }

    // Calculate the canonical weights
    std::vector<double> weights = ca->calc_weights(energies, parser.get_as<double>("beta"));

    // Print the weights
    for (unsigned int i=0; i<weights.size(); ++i) {
        if (white_spaces.count(i)>0)
            (*output) << white_spaces[i];

        (*output) << weights[i];
    }

    if (white_spaces.count(weights.size())>0)
        (*output) << white_spaces[weights.size()];

    // Cleanup
    delete ca;
    delete input_file;
    delete output_file;
}

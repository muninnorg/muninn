// combine_logs.cpp
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
#include <vector>

#include "../details/OptionParser.h"

#include "muninn/common.h"
#include "muninn/Factories/CGEfactory.h"
#include "muninn/MLE/MLE.h"
#include "muninn/Binners/NonUniformBinner.h"
#include "muninn/utils/StatisticsLogReader.h"
#include "muninn/utils/ArrayAligner.h"

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("Program for combining different log files (histories) with same binning and recalculate the lnG.", "The list of Muninn statistics log files to combine (e.g. muninn_0.txt muninn_1.txt)");
    parser.add_option("-o", "output_filename", "The filename for the output log file (e.g. muninn_combined.txt)", OptionParser::REQUIRED);
    parser.add_option("-m", "memory", "The number of histogram to read for each file", Muninn::to_string(Muninn::CGEfactory::Settings().memory));
    parser.add_option("-t", "threshold", "Threshold for squared error used when aligning arrays ", "1E-6");
    parser.parse_args(argc, argv);

    const double threshold = parser.get_as<double>("threshold");
    Muninn::StatisticsLogger statistics_logger(parser.get("output_filename"), Muninn::StatisticsLogger::ALL, Muninn::CGEfactory::Settings().log_precision);

    // Setup the estimator and related objects
    Muninn::MLE estimator(Muninn::CGEfactory::Settings().min_count, std::numeric_limits<unsigned int>::max(), true);
    Muninn::Estimate* estimate = NULL;
    Muninn::History* history = NULL;
    Muninn::DArray binning(1);
    Muninn::NonUniformBinner binner(binning);


    // Read the log files
    for (size_t file_index=0; file_index<parser.get_additional_arguments().size(); ++file_index) {
        // Read the log file
        Muninn::MessageLogger::get().info("Processing: " + parser.get_additional_arguments().at(file_index));
        Muninn::StatisticsLogReader statistics_log_reader(parser.get_additional_arguments().at(file_index), parser.get_as<unsigned int>("memory"));

        // Create the objects used by the estimator
        if (history==NULL) {
            std::vector<Muninn::Index> shape = statistics_log_reader.get_Ns().back().second.get_shape();
            estimate = estimator.new_estimate(shape);
            history = estimator.new_history(shape);
            binning = statistics_log_reader.get_binnings().back().second;
            binner = Muninn::NonUniformBinner(binning);
            statistics_logger.add_loggable(history);
            statistics_logger.add_loggable(estimate);
            statistics_logger.add_loggable(&binner);
        }


        for (unsigned int i = 0; i < statistics_log_reader.get_Ns().size(); ++i) {
            const Muninn::CArray& ith_N = statistics_log_reader.get_Ns().at(i).second;
            const Muninn::DArray& ith_lnw = statistics_log_reader.get_lnws().at(i).second;
            const Muninn::DArray& ith_binning = statistics_log_reader.get_binnings().at(i).second;

            // Check if the arrays should be extended based on the binning
            if (ith_binning.get_shape(0) < binning.get_shape(0)) {
                std::pair<unsigned int, double> offset_and_score = Muninn::ArrayAligner::calculate_alignment_offset_and_score(binning, ith_binning);
                if (offset_and_score.second > threshold) {
                    Muninn::MessageLogger::get().error("Threshold exceed for " + statistics_log_reader.get_binnings().at(i).first + " in " + parser.get_additional_arguments().at(file_index));
                    return EXIT_FAILURE;
                }
                std::pair<unsigned int, unsigned int> offsets = Muninn::ArrayAligner::calculate_alignment_offsets(binning, ith_binning);
                Muninn::CArray extended_N = ith_N.extended(offsets.first, offsets.second);
                Muninn::DArray extended_lnw = ith_lnw.extended(offsets.first, offsets.second);
                history->add_histogram(new Muninn::Histogram(extended_N, extended_lnw));
            }
            else if (ith_binning.get_shape(0) > binning.get_shape(0)) {
                std::pair<unsigned int, double> offset_and_score = Muninn::ArrayAligner::calculate_alignment_offset_and_score(ith_binning, binning);
                if (offset_and_score.second > threshold) {
                    Muninn::MessageLogger::get().error("Threshold exceed for " + statistics_log_reader.get_binnings().at(i).first + " in " + parser.get_additional_arguments().at(file_index));
                    return EXIT_FAILURE;
                }
                std::pair<unsigned int, unsigned int> offsets = Muninn::ArrayAligner::calculate_alignment_offsets(ith_binning, binning);

                binning = ith_binning;
                estimate->extend(Muninn::newvector<Muninn::Index>(offsets.first), Muninn::newvector<Muninn::Index>(offsets.second));
                history->extend(Muninn::newvector<Muninn::Index>(offsets.first), Muninn::newvector<Muninn::Index>(offsets.second));
                binner = Muninn::NonUniformBinner(binning);

                history->add_histogram(new Muninn::Histogram(ith_N, ith_lnw));
            }
            else {
                std::pair<unsigned int, double> offset_and_score = Muninn::ArrayAligner::calculate_alignment_offset_and_score(binning, ith_binning);
                if (offset_and_score.second > threshold) {
                    Muninn::MessageLogger::get().error("Threshold exceed for " + statistics_log_reader.get_binnings().at(i).first + " in " + parser.get_additional_arguments().at(file_index));
                    return EXIT_FAILURE;
                }
                history->add_histogram(new Muninn::Histogram(ith_N, ith_lnw));
            }

            // Estimate new weigts
            estimator.estimate(*history, *estimate, &binner);
            statistics_logger.log();
        }

    }

    return EXIT_SUCCESS;
}

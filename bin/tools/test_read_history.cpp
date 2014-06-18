// test_read_history.cpp
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

#include "muninn/utils/StatisticsLogReader.h"
#include "muninn/Factories/CGEfactory.h"


#include "../details/OptionParser.h"

int main(int argc, char *argv[]) {
    // Setup the option parser
    OptionParser parser("Test if a Muninn statistics log file can be read.");
    parser.add_option("-m", "max_hist", "The maximal number of histograms to read", "0");
    parser.add_option("-r", "read_statistics_log", "Read a Muninn statistics log file");
    parser.parse_args(argc, argv);

    // Try to read the logfile
    std::cout << "Try to read log file: " << parser.get("read_statistics_log") << std::endl;

    try {
        Muninn::StatisticsLogReader statistics_log_reader = Muninn::StatisticsLogReader(parser.get("read_statistics_log"), parser.get_as<unsigned int>("max_hist"));
    }
    catch (Muninn::MessageException& exception) {
        parser.parser_error(exception.what());
        return EXIT_FAILURE;
    }

    // Try to setup a new CGE object based on the history
    std::cout << "Try to create a CGE object from log file: " << parser.get("read_statistics_log") << std::endl;

    Muninn::CGEfactory::Settings settings;
    settings.statistics_log_filename = "/dev/null";
    settings.read_statistics_log_filename = parser.get("read_statistics_log");
    settings.verbose = 3;

    Muninn::CGE *cge = Muninn::CGEfactory::new_CGE(settings);


    return EXIT_SUCCESS;
}

// StatisticsLogReader.cpp
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

#include <muninn/utils/StatisticsLogReader.h>
#include <muninn/utils/utils.h>

namespace Muninn {

void StatisticsLogReader::read(std::istream& input) {
    int line_counter = 0;

    // Temporary variables for strings
    std::deque<std::pair<std::string,std::string> > Ns_string;
    std::deque<std::pair<std::string,std::string> > lnws_string;
    std::deque<std::pair<std::string,std::string> > lnGs_string;
    std::deque<std::pair<std::string,std::string> > lnG_supports_string;
    std::deque<std::pair<std::string,std::string> > binnings_string;
    std::deque<std::pair<std::string,std::string> > bin_widths_string;

    // Read the input
    while (input.good()) {
        // Parse the input file line by line
        std::string line;
        std::getline(input, line);
        line = strip(line);
        ++line_counter;

        // Skip blank lines
        if (line!="") {
            // Split the line at first equal signe (=)
            std::string::size_type pos = line.find_first_of("=");

            if (pos != std::string::npos && pos<line.size()-1) {
                std::string name = strip(line.substr(0, pos-1));
                std::string array = line.substr(pos+1);

                // Decide on which array has been read
                if (name.substr(0,1)=="N") {
                    Ns_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && Ns_string.size()>max_hist)
                        Ns_string.pop_front();
                }
                else if (name.substr(0,3)=="lnw") {
                    lnws_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && lnws_string.size()>max_hist)
                        lnws_string.pop_front();
                }
                else if (name.substr(0,11)=="lnG_support") {
                    lnG_supports_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && lnG_supports_string.size()>max_hist)
                        lnG_supports_string.pop_front();
                }
                else if (name.substr(0,3)=="lnG") {
                    lnGs_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && lnGs_string.size()>max_hist)
                        lnGs_string.pop_front();
                }
                else if (name.substr(0,7)=="binning") {
                    binnings_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && binnings_string.size()>max_hist)
                        binnings_string.pop_front();
                }
                else if (name.substr(0,10)=="bin_widths") {
                    bin_widths_string.push_back(std::pair<std::string,std::string>(name,array));
                    if (max_hist>0 && binnings_string.size()>max_hist)
                        binnings_string.pop_front();
                }
                else {
                    MessageLogger::get().warning("When reading statistics log, found unknown identifier \"" + name + "\" at line " + to_string(line_counter) + ".");
                }
            }
            else {
                MessageLogger::get().warning("When reading statistics log, line " + to_string(line_counter) + " did not contain a equal sign (=).");
            }
        }
    }

    // Convert the read data to arrays
    string_to_array(Ns_string, Ns);
    string_to_array(lnws_string, lnws);
    string_to_array(lnGs_string, lnGs);
    string_to_array(lnG_supports_string, lnG_supports);
    string_to_array(binnings_string, binnings);
    string_to_array(bin_widths_string, bin_widths);
}

} // namespace Muninn

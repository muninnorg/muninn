// StatisticsLogger.h
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

#ifndef MUNINN_STATISTICS_LOGGER_H
#define MUNINN_STATISTICS_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/Loggable.h"

namespace Muninn {

/// Class for logging the statistics collected in the history, histograms,
/// the corresponding weights and entropy estimates.
class StatisticsLogger {
public:
    /// The modes for the logger.
    enum Mode {NONE=0,  ///< Nothing is logged.
               ALL,     ///< Log all estimates and the full history.
               CURRENT, ///< Log only the current entropy estimate and current history (works only for the MultiHistogramHistory).
               SIZE     ///< Indicator value.
              };

    /// String representation of the different log modes (Mode).
    static const std::string ModeNames[];

    /// Constructor for the logger.
    ///
    /// \param filename The filename to write the log to.
    /// \param mode The logging mode (see Mode for details).
    /// \param precision The precision (number of significant digits) used when
    ///                  writing floating point values to the log file.
    /// \param append_to_file Weather the output should be appended to the log file. If false the log file is overwritten.
    /// \param counter_offset The start value (offset) for the counter setting the entry number.

    StatisticsLogger(const std::string &filename, Mode mode=CURRENT, int precision=10, bool append_to_file=false, unsigned int counter_offset=0) :
        filename(filename), mode(mode), precision(precision), counter(0), loggables(), entry_queue() {

        // Clear the output file
        if (!append_to_file && mode!=NONE) {
            outstream.open(filename.c_str());
            outstream.close();
        }

        // Set the counter off, if required
        if (counter_offset>0 && mode==ALL) {
            counter = counter_offset;
        }

    }

    /// Default destructor.
    ~StatisticsLogger() {}

    /// Write entries for all Loggable objects. In the mode Mode::ALL data is
    /// appended to the file, while in Mode::CURRENT the file is rewritten at
    /// every call.
    void log() {
        if (mode != NONE) {
            // Collect entries of all Loggables.
            for (std::vector<const Loggable*>::iterator it=loggables.begin(); it!=loggables.end(); ++it) {
                (*it)->add_statistics_to_log(*this);
            }

            // Write the entries to file
            write_entry_queue();

            // Clear the queue and additional vector
            entry_queue.clear();
            last_entry_in_queue.clear();

            // Set the value of the counter depending on the mode.
            if (mode==ALL) {
                ++counter;
            }
            else {
                counter = 0;
            }
        }
    }

    /// Add an object to be logged to the StatisticsLogger. Every time the
    /// function is called, the function Loggable::add_statistics_to_log() will
    /// be called on added objects.
    ///
    /// \param loggable The loggable object to be added to the logger.
    void add_loggable(const Loggable *loggable) {
        if (loggable!=NULL) {
            loggables.push_back(loggable);
        }
    }


    /// Add a entry to the log. This function is normally called by an object
    /// of the Loggable class. The entry in the log file will look as follows:
    ///
    ///    [name][counter] = [array]
    ///
    /// \param name The name of the entry.
    /// \param array The array for the entry.
    template<typename T>
    void add_entry(const std::string& name, const TArray<T>& array) {
        if (mode==ALL) {
            if (last_entry_in_queue.count(name) == 0) {
                entry_queue.push_back(std::pair<std::string, std::string>(name + to_string(counter), array.write(precision)));
                last_entry_in_queue[name] = entry_queue.size() - 1;
            }
            else {
                entry_queue.at(last_entry_in_queue[name]) = std::pair<std::string, std::string>(name + to_string(counter), array.write(precision));
            }
        }
        else {
            if (last_entry_in_queue.count(name) > 0) {
                ++counter;
                last_entry_in_queue.clear();
            }

            entry_queue.push_back(std::pair<std::string, std::string>(name + to_string(counter), array.write(precision)));
            last_entry_in_queue[name] = entry_queue.size() - 1;
        }
    }


private:
    const std::string filename;  ///< The filename of the file to write the log to.
    const Mode mode;             ///< The logging mode.
    const int precision;         ///< The precision (number of significant digits) used when writing floating point values to the log file.
    unsigned int counter;        ///< Counter for setting the entry number (update number) when writing to the log file.
    std::ofstream outstream;     ///< The output stream to write the log to.

    std::vector<const Loggable*> loggables;                             ///< List of classes that should be logged
    std::vector<std::pair<std::string, std::string> > entry_queue;      ///< Queued arrays waiting to be written
    std::map<std::string, size_t> last_entry_in_queue;

    /// Private method for writing the current contents of the queue to the
    /// log file.
    void write_entry_queue() {
    	// TODO: Sort the output

    	// Open the output file and set the pression
    	std::ios_base::openmode open_mode = mode==ALL ? std::ios_base::app : std::ios_base::out;
    	outstream.open(filename.c_str(), open_mode);

    	// Write to the output file
    	for (std::vector<std::pair<std::string, std::string> >::const_iterator it=entry_queue.begin(); it!=entry_queue.end(); ++it) {
    		outstream << it->first << " = " << it->second <<  std::endl;
    	}

        outstream << std::endl << std::flush;
        outstream.close();
    }
};

} // namespace Muninn

#endif // MUNINN_STATISTICS_LOGGER_H

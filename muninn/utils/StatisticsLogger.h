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
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Histogram.h"
#include "muninn/History.h"
#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/Estimate.h"
#include "muninn/Binner.h"

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
    StatisticsLogger(const std::string &filename, Mode mode=CURRENT, int precision=10) :
        filename(filename), mode(mode), precision(precision), counter(0) {
        if (mode==ALL) {
            outstream.open(filename.c_str());
            outstream << std::setprecision(precision);
        }
    }

    /// Default destructor.
    ~StatisticsLogger() {
        if (mode==ALL)
            outstream.close();
    }

    /// Write data to the logfile. In the mode Mode::ALL data is appended to
    /// the file, while in Mode::CURRENT the file is rewritten at every call.
    ///
    /// The Mode::CURRENT mode assumes that the history is of type
    /// MultiHistogramHistory, and write the full contents of the history
    /// to the file, together with the newest entropy estimate and
    /// and binning details (if present).
    ///
    /// The Mode::ALL mode does not use the history, it just appends the
    /// newest collected histogram to the history, along with the nest entropy
    /// estimate and binning details (if present).
    ///
    /// \param new_histogram The newest histogram that has just been added to
    ///                      the history.
    /// \param new_history_base The newest history, which has been used to make
    ///                         a new estimate of the entropy.
    /// \param new_estimate The newest estimate of the entropy (lnG)
    /// \param binner A pointer to the binner that has been used. The bin edges
    ///               and bin widths are written to the file (if the binner is
    ///               not NULL).
    void log(const Histogram &new_histogram, const History &new_history_base, const Estimate &new_estimate, const Binner *binner=NULL) {
        if (mode==ALL) {
            // Write the data
            outstream << "N" << counter << " = " << new_histogram.get_N() <<  std::endl;
            outstream << "lnw" << counter << " = " << new_histogram.get_lnw() << std::endl;
            outstream << "lnG" << counter << " = " << new_estimate.get_lnG() << std::endl;
            outstream << "lnG_support" << counter << " = " << new_estimate.get_lnG_support() << std::endl;

            if (binner) {
                outstream << "binning" << counter << " = " << binner->get_binning() << std::endl;
                outstream << "bin_widths" << counter << " = " << binner->get_bin_widths() << std::endl;
            }

            // Flush the stream and update the counter
            outstream << std::endl << std::flush;
            ++counter;
        }
        else if(mode==CURRENT) {
            // Cast the history to be a MultiHistogramHistory
            const MultiHistogramHistory& new_history = MultiHistogramHistory::cast_from_base(new_history_base, "The StatisticsLogger only support MultiHistogramHistory in current mode");

            // Open and clean the output file
            outstream.open(filename.c_str());
            outstream << std::setprecision(precision);

            // Write the data
            counter = 0;
            for (MultiHistogramHistory::const_reverse_iterator it=new_history.rbegin(); it!=new_history.rend(); ++it) {
                outstream << "N" << counter << " = " << (*it)->get_N() <<  std::endl;
                outstream << "lnw" << counter << " = " << (*it)->get_lnw() << std::endl;
                outstream << std::endl;
                ++counter;
            }

            outstream << "lnG" << (counter-1) << " = " << new_estimate.get_lnG() << std::endl;
            outstream << "lnG_support" << (counter-1) << " = " << new_estimate.get_lnG_support() << std::endl;
            if (binner) {
                outstream << "binning" << (counter-1) << " = " << binner->get_binning() << std::endl;
                outstream << "bin_widths" << (counter-1) << " = " << binner->get_bin_widths() << std::endl;
            }

            // Close the file
            outstream.close();
        }
    }

    /// Input stream operator for the Mode types.
    friend std::istream &operator>>(std::istream &input, Mode &mode) {
         std::string raw_string;
         input >> raw_string;

         for (unsigned int i=0; i<SIZE; ++i) {
              if (raw_string == ModeNames[i]) {
                   mode = Mode(i);
              }
         }
         return input;
    }

    /// Output stream operator for the Mode types.
    friend std::ostream &operator<<(std::ostream &o, const Mode &mode) {
         o << Muninn::StatisticsLogger::ModeNames[static_cast<unsigned int>(mode)];
         return o;
    }

private:
    const std::string filename;  ///< The filename of the file to write the log to.
    const Mode mode;             ///< The logging mode.
    const int precision;         ///< The precision (number of significant digits) used when writing floating point values to the log file.
    unsigned int counter;        ///< Counter for setting the entry number (update number) when writing to the log file.
    std::ofstream outstream;     ///< The output stream to write the log to.
};

} // namespace Muninn

#endif // MUNINN_STATISTICS_LOGGER_H

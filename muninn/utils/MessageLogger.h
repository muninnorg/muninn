// MessageLogger.h
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

#ifndef MUNINN_MESSAGELOGGER_H
#define MUNINN_MESSAGELOGGER_H

#include <iostream>
#include <string>

#include "muninn/common.h"

namespace Muninn {

/// A class for logging messages in Muninn. Messages can be written to any type
/// of steams:
///
/// If the verbose level is set, errors are written to either stdout or stderr
/// depending on the verbose level:
///  - For verbose=0 no messages are reported.
///  - For verbose=1 errors are written to stderr and warning to stdout.
///  - For verbose=2 information messages are written to stdout (errors and warning as verbose=1).
///  - For verbose=3 debug messages are written to stdout (information, errors and warning as verbose=2).
///
/// The class also has a constructor for setting the individual streams.
class MessageLogger {
public:

    // Constructor where the individual streams are set manually.
    ///
    /// \param info_stream The stream used for reporting information messages.
    /// \param debug_stream The stream used for writing debug statements.
    /// \param warning_stream The stream used for warning.
    /// \param error_stream The stream used for error messages.
    MessageLogger(std::ostream *info_stream, std::ostream *debug_stream, std::ostream *warning_stream, std::ostream *error_stream) :
            info_stream(info_stream), debug_stream(debug_stream), warning_stream(warning_stream), error_stream(error_stream) {}

    /// Constructor where the streams are set based on the verbose level.
    ///
    /// \param verbose The verbose level.
    MessageLogger(unsigned int verbose=2) : info_stream(NULL), debug_stream(NULL), warning_stream(NULL), error_stream(NULL) {
        set_verbose(verbose);
    }

    /// Set the the streams based on a verbose level.
    ///
    /// \param verbose The verbose level.
    void set_verbose(unsigned int verbose=2) {
        info_stream = debug_stream = warning_stream = error_stream = NULL;
        if (verbose >= 1) {
            error_stream = &std::cerr;
            warning_stream = &std::cout;
        }
        if (verbose >= 2)
            info_stream = &std::cout;
        if (verbose >= 3)
            debug_stream = &std::cout;
    }

    /// Write a debug message to the debug stream.
    ///
    /// \param message The debug message.
    void debug(std::string message) {
        if (debug_stream)
            (*debug_stream) << "# MUNINN DEBUG: " << message << std::endl;
    }

    /// Write a information message to the information stream.
    ///
    /// \param message The information message.
    void info(std::string message) {
        if (info_stream)
            (*info_stream) << "# MUNINN: " << message << std::endl;
    }

    /// Write a warning message to the warning stream.
    ///
    /// \param message The warning message.
    void warning(std::string message) {
        if (warning_stream)
            (*warning_stream) << "# MUNINN WARNING: " << message << std::endl;
    }

    /// Write a error message to the error stream.
    ///
    /// \param message The error message.
    void error(std::string message) {
        if (error_stream)
            (*error_stream) << "# MUNINN ERROR: " << message << std::endl;
    }

    /// Static function of accessing a global MessageLogger. Note that
    /// accessing this global logger is not thread safe!
    ///
    /// \return A reference to a global MessageLogger
    static MessageLogger& get() {
        return logger;
    }

private:
    std::ostream *info_stream;     ///< The stream used for reporting information messages.
    std::ostream *debug_stream;    ///< The stream used for writing debug statements.
    std::ostream *warning_stream;  ///< The stream used for warning.
    std::ostream *error_stream;    ///< The stream used for error messages.

    static MessageLogger logger;   ///< A global MessageLogger.
};

} // namespace Muninn

#endif // MUNINN_MESSAGELOGGER_H

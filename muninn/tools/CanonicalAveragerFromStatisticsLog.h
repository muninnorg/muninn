// CanonicalAveragerFromStatisticsLog.h
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

#ifndef MUNINN_CANONICAL_AVERAGER_FROM_STATISTICS_LOG_H
#define MUNINN_CANONICAL_AVERAGER_FROM_STATISTICS_LOG_H

#include <vector>
#include <string>

#include "muninn/tools/CanonicalAverager.h"
#include "muninn/Binners/NonUniformBinner.h"
#include "muninn/utils/StatisticsLogReader.h"

namespace Muninn {

/// This class inherits from the CanonicalAverager and adds a alternative
/// constructor such that the class can be constructed based on the filename of
/// an Muninn statistics log file.
///
/// See base class (CanonicalAverager) for details on how the canonical average
/// is calculated.
class CanonicalAveragerFromStatisticsLog : public CanonicalAverager {
public:

    /// Constructors the class based on a Muninn statistics log filename.
    ///
    /// \param filename The filename of a Muninn statistics log.
    CanonicalAveragerFromStatisticsLog(const std::string& filename) : CanonicalAverager(), log_reader(filename, 1) {
        // Check that the log file contains the required data
        if (log_reader.get_binnings().size()==0 || log_reader.get_lnGs().size()==0 || log_reader.get_lnG_supports().size()==0)
            throw MessageException("Cannot make CanonicalAverager from the statistics log, some of the required data is missing in the file.");

        // Extract data from the log file
        set_binner_from_array(log_reader.get_binnings().back().second);
        lnG = &log_reader.get_lnGs().back().second;
        lnG_support = &log_reader.get_lnG_supports().back().second;
    }

protected:
    StatisticsLogReader log_reader;  ///< The object for reading the statistics log.
};

} // namespace Muninn

#endif /* MUNINN_CANONICAL_AVERAGER_FROM_STATISTICS_LOG_H */

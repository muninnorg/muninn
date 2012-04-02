// Loggable.h
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

#ifndef MUNINN_LOGGABLE_H_
#define MUNINN_LOGGABLE_H_

#include "muninn/common.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

// Forward declarations
class StatisticsLogger;

/// The Loggable abstract class is the interface for classes implementing
/// that add entries to the log file through the StatisticsLogger class.
class Loggable {
public:

    /// Add an entries to the statistics log. Inside the function entries
    /// can be added to the log file by calling add_entry on the
    /// statistics_logger class.
    ///
    /// \param statistics_logger The logger to add an entry to.
	virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const = 0;

};

} // namespace Muninn

#endif // MUNINN_LOGGABLE_H_

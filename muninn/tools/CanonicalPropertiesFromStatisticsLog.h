// CanonicalPropertiesFromStatisticsLog.h
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

#ifndef MUNINN_CANONICAL_PROPERTIES_FROM_STATISTICS_LOG_H_
#define MUNINN_CANONICAL_PROPERTIES_FROM_STATISTICS_LOG_H_

#include <string>

#include "muninn/tools/CanonicalProperties.h"
#include "muninn/utils/StatisticsLogReader.h"

namespace Muninn {

/// This class inherits from the CanonicalProperties and adds an alternative
/// constructor such that the class can be constructed based on the filename of
/// an Muninn statistics log file.
///
/// See base class (CanonicalProperties) for details on the canonical properties.
class CanonicalPropertiesFromStatisticsLog : public CanonicalProperties {
public:

    /// Constructors the class based on a Muninn statistics log filename.
    ///
    /// \param filename The filename of a Muninn statistics log.
    CanonicalPropertiesFromStatisticsLog(const std::string& filename) : CanonicalProperties(), bin_centers(NULL), log_reader(filename, 1) {
        // Check that the log file contains the required data
        if (log_reader.get_binnings().size()==0 || log_reader.get_lnGs().size()==0 || log_reader.get_lnG_supports().size()==0) {
            throw MessageException("Cannot make CanonicalProperties from the statistics log, some of the required data is missing in the file.");
        }

        // Set the lnG and lnG_support
        lnG = &log_reader.get_lnGs()[0].second;
        lnG_support = &log_reader.get_lnG_supports()[0].second;

        // Calculate the bin centers (the energy)
        bin_centers = new DArray(lnG->get_shape());
        const DArray &binning = log_reader.get_binnings()[0].second;

        for(unsigned int i=1; i < this->lnG->get_asize(); ++i) {
            (*bin_centers)(i-1) = binning(i-1) + 0.5*(binning(i)-binning(i-1));
        }

        // Set the energy variable
        energies = bin_centers;
    }

    /// Default destructor.
    virtual ~CanonicalPropertiesFromStatisticsLog() {
        delete bin_centers;
    }

protected:
    DArray *bin_centers;             ///< A non-const pointer to the energies of the the bin centers.
    StatisticsLogReader log_reader;  ///< The object for reading the statistics log.
};

} // namespace Muninn

#endif /* MUNINN_CANONICAL_PROPERTIES_FROM_STATISTICS_LOG_H_ */

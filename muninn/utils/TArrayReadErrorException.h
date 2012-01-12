// TArrayReadErrorException.h
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

#ifndef MUNINN_TARRAY_READ_ERROR_EXCEPTION_H_
#define MUNINN_TARRAY_READ_ERROR_EXCEPTION_H_

#include <exception>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/utils.h"

namespace Muninn {

/// Exception thrown if it fails to read an TArray from a files.
class TArrayReadErrorException : public MuninnException {
public:

    /// Constructor for the exception.
    ///
    /// \param problem A text describing the problem.
    TArrayReadErrorException(const std::string &problem) : problem(problem) {
        description = "Error reading TArray: " + problem;
    }

    /// Default destructor.
    virtual ~TArrayReadErrorException() throw() {};

    /// Returns a character string describing the error.
    virtual const char* what() const throw() {
        return description.c_str();
    }

private:
    std::string problem;     ///< A text describing the problem.
    std::string description; ///< A general text describing the error.
};

} // namespace Muninn

#endif // MUNINN_TARRAY_READ_ERROR_EXCEPTION_H_

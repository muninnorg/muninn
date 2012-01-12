// TArrayMismatchSizeException.h
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

#ifndef MUNINN_TARRAY_MISMATCH_SIZE_EXCEPTION_H_
#define MUNINN_TARRAY_MISMATCH_SIZE_EXCEPTION_H_

#include <exception>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/utils.h"

namespace Muninn {

/// Exception thrown by functions that assumes that the size of two arrays
/// are the same.
class TArrayMismatchSizeException : public MuninnException {
public:

    /// Constructor for the exception.
    ///
    /// \param size1 The size of the first array.
    /// \param size2 The size of the second array.
    TArrayMismatchSizeException(unsigned int size1, unsigned int size2) : size1(size1), size2(size2) {
        description = "Mismatch in size for TArrays; the sizes are " + to_string(size1) + " and " + to_string(size2) + ".";
    }

    /// Default destructor.
    virtual ~TArrayMismatchSizeException() throw() {};

    /// Returns a character string describing the error.
    virtual const char* what() const throw() {
        return description.c_str();
    }

    /// Get the size of the first array.
    ///
    /// \return The size of the first array.
    const unsigned int& get_size1() const {
        return size1;
    }

    /// Get the size of the second array.
    ///
    /// \return The size of the second array.
    const unsigned int& get_size2() const {
        return size2;
    }

private:
    unsigned int size1;       ///< The size of the first array.
    unsigned int size2;       ///< The size of the second array.
    std::string description;  ///< A character string describing the error.
};

} // namespace Muninn

#endif // MUNINN_TARRAY_MISMATCH_SIZE_EXCEPTION_H_

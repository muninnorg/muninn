// TArrayMismatchShapeException.h
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

#ifndef MUNINN_TARRAY_MISMATCH_SHAPE_EXCEPTION_H_
#define MUNINN_TARRAY_MISMATCH_SHAPE_EXCEPTION_H_

#include <exception>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/utils.h"

namespace Muninn {

/// Exception thrown by functions that assumes that the shape of two arrays
/// are the same.
class TArrayMismatchShapeException : public MuninnException {
public:

    /// Constructor for the exception.
    ///
    /// \param shape1 The shape of the first array.
    /// \param shape2 The shape of the second array.
    TArrayMismatchShapeException(const std::vector<unsigned int> &shape1, const std::vector<unsigned int> &shape2) throw() : shape1(shape1), shape2(shape2) {
        description = "Mismatch in shape for TArrays; the shapes are " + to_string(shape1) + " and " + to_string(shape2) + ".";
    }

    /// Default destructor.
    virtual ~TArrayMismatchShapeException() throw() {};

    /// Returns a character string describing the error.
    virtual const char* what() const throw() {
        return description.c_str();
    }

    /// Get the shape of the first array.
    ///
    /// \return The shape of the first array.
    const std::vector<unsigned int>& get_shape1() const {
        return shape1;
    }

    /// Get the shape of the second array.
    ///
    /// \return The shape of the second array.
    const std::vector<unsigned int>& get_shape2() const {
        return shape2;
    }

private:
    std::vector<unsigned int> shape1; ///< The shape of the first array.
    std::vector<unsigned int> shape2; ///< The shape of the second array.
    std::string description;          ///< A character string describing the error.
};

} // namespace Muninn

#endif // MUNINN_TARRAY_MISMATCH_SHAPE_EXCEPTION_H_

// ErrorFunction.h
// Copyright (c) 2011 Jes Frellsen
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

#ifndef MUNINN_NEWTON_LINESEARCH_H_
#define MUNINN_NEWTON_LINESEARCH_H_

#include "Eigen/Core"

namespace Muninn {
namespace Newton {

/// Error function for the Newton root finder.
///
/// \tparam Vector The type used for vectors used in the class
/// \tparam Function The type of the functor used as function
template <typename Vector, typename Scalar, class Function>
class ErrorFunction {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Constructor for the error function. For a function F, the error is
    /// defined as F*F/2 - where * is the dot product.
    ///
    /// \param function The function to evaluate the error of.
    /// \param n The number of equations (dimensionality).
    ErrorFunction(Function& function, const unsigned int& n) :
        function(function),
        x(n),
        function_value(n) {
        error = std::numeric_limits<Scalar>::infinity();
    }

    virtual ~ErrorFunction() {};

    /// Calculate the error in a given point.
    ///
    /// \param x The point to evaluate the error in.
    /// \tparam The type of x.
    /// \return The error.
    template <typename Derived>
    Scalar operator()(const Eigen::MatrixBase<Derived>& x) {
        this->x = x;
        function(x, function_value);
        error = 0.5 * function_value.dot(function_value);
        return error;
    }

    /// Get the latest evaluated function value.
    ///
    /// \return The function value.
    const Vector* get_function_value() const {
        return &function_value;
    }

    /// Get the latest evaluated error value.
    ///
    /// \return The error value.
    const Scalar* get_error_value() const {
        return &error;
    }

private:
    Function& function;      ///< Reference to the function.
    Vector x;                ///< The latest used x value.
    Vector function_value;   ///< The latest evaluated function value.
    Scalar error;            ///< The latest evaluated error value.
};

} // namespace Newton
} // namespace Muninn


#endif /* MUNINN_NEWTON_LINESEARCH_H_ */

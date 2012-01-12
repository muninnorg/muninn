// newton.cpp
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

#include "Eigen/Core"

#include "newton.h"
#include "newton/NewtonRootFinder.h"

namespace Muninn {

/// A functor wrapper for function method of a NonlinearEquation object
class FunctionFunctorWrapper {
public:
    /// Constructor for the wrapper
    ///
    /// \param non_linear_equation The object to be wrapped
    FunctionFunctorWrapper(NonlinearEquation& non_linear_equation) :
        non_linear_equation(non_linear_equation) {}

    /// The function operator.
    ///
    /// \param x The x value.
    /// \param f The calculated function value.
    void operator()(const Eigen::VectorXd &x, Eigen::VectorXd &f) {
        // Note  that the const cast is reasonable, since X is declared const and
        // the constructor of DArray does not modify the contents of the storage.
        const DArray X(newvector<Index>(x.size()), const_cast<double*>(x.data()));
        DArray F(newvector<Index>(f.size()), f.data());
        non_linear_equation.function(X, F);
    }

private:
    NonlinearEquation& non_linear_equation;
};

/// A functor wrapper for Jacobian method of a NonlinearEquation object
class JacobianFunctorWrapper {
public:
    /// Constructor for the wrapper
    ///
    /// \param non_linear_equation The object to be wrapped
    JacobianFunctorWrapper(NonlinearEquation& non_linear_equation) :
        non_linear_equation(non_linear_equation) {}

    /// The Jacobian operator.
    ///
    /// \param x The x value.
    /// \param f The function value in x.
    /// \param j The returned calcluated Jacobian value in x.
    void operator()(const Eigen::VectorXd &x, const Eigen::VectorXd &f, Eigen::MatrixXd &j) {
        // Note that the const casts are reasonable, since X and J are declared const and
        // the constructor of DArray does not modify the contents of the storage.
        const DArray X(newvector<Index>(x.size()), const_cast<double*>(x.data()));
        const DArray F(newvector<Index>(f.size()), const_cast<double*>(f.data()));
        DArray J(newvector<Index>(j.rows(), j.cols()), j.data());
        non_linear_equation.jacobian(X, F, J);
    }

private:
    NonlinearEquation& non_linear_equation;
};

int newton(DArray &X, NonlinearEquation &eqn) {
    // Check that X is one dimensional
    assert(X.nonempty() && X.get_ndims()==1);

    // Setup the functors
    FunctionFunctorWrapper function_functor_wrapper(eqn);
    JacobianFunctorWrapper jacobian_functor_wrapper(eqn);

    // Get the number of equations
    Index n = X.get_shape(0);

    // Make an vector and copy the values of X to it
    Eigen::VectorXd x(n);

    for (Index i=0; i<n; ++i) {
        x(i) = X(i);
    }

    // Call newt_hess function
    Newton::NewtonRootFinder<double, Eigen::VectorXd, Eigen::MatrixXd> root_finder(1.0E-9, 1.0E-6, 1.0E-8, 100, 75, 1E-4);
    Newton::NewtonRootFinder<double, Eigen::VectorXd, Eigen::MatrixXd>::ReturnValue return_value;

    return_value = root_finder.newton(x, x, function_functor_wrapper, jacobian_functor_wrapper);

    // Copy the result to X
    for (Index i=0; i<n; ++i) {
        X(i) = x(i);
    }

    // Return the status of the root finder
    return static_cast<int>(return_value);
}

} // namespace Muninn

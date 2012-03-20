// NewtonRootFinder.h
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

#ifndef MUNINN_NEWTON_NEWTONROOTFINDER_H_
#define MUNINN_NEWTON_NEWTONROOTFINDER_H_

#include <limits>

#include "Eigen/Core"
#include "Eigen/Dense"

#include "LineSearchAlgorithm.h"
#include "ErrorFunction.h"

/// \namespace Muninn::Newton Namespace for the implementation of Newtons
/// method for solving sets of nonlinear equations.

namespace Muninn {
namespace Newton {

/// Implements a root finder for sets of nonlinear equations,
/// \f[
///   \mathbf{F}(\mathbf{x}) = \mathbf{0} ,
/// \f]
/// where \f$\mathbf{x}\f$ is a vector and \f$\mathbf{F}\f$ is a nonlinear
/// vector function. The algorithm is described in "Numerical Recipes in C++".
///
/// \tparam Scalar The type used for scalars in the algorithm.
/// \tparam Vector The type used for vectors in the algorithm.
/// \tparam Array The type used for arrays in the algorithm.
template <typename Scalar, typename Vector, typename Array>
class NewtonRootFinder {
public:

    /// Constructor for the Newtoon root fineder.
    ///
    /// \param tolerance_x Tolerance used for testing for convergence in x - if negative initialized to the minimal epsilon for the scalar type.
    /// \param tolerance_function Tolerance used for testing for convergence in the function value.
    /// \param tolerance_gradient Tolerance used when testing if the gradient is zero (spurious convergence).
    /// \param max_step_factor Scaling factor used for setting the maximal step size.
    /// \param max_iterations Maximal number of iterations used in the algorithm.
    /// \param alpha Required average rate of decrease in f used by the line search algorithm.
    NewtonRootFinder(const Scalar tolerance_x = -1,
                     const Scalar tolerance_function = 1E-8,
                     const Scalar tolerance_gradient = 1E-12,
                     const Scalar max_step_factor = 100,
                     const unsigned int max_iterations = 200,
                     const Scalar& alpha=1E-4) :
                         tolerance_x(tolerance_x>=0 ? tolerance_x : std::numeric_limits<Scalar>::epsilon()),
                         tolerance_function(tolerance_function),
                         tolerance_gradient(tolerance_gradient),
                         max_step_factor(max_step_factor),
                         max_iterations(max_iterations),
                         line_search_algorithm(alpha, this->tolerance_x) {}

    /// Return values for the Newton root finding algorithm
    enum ReturnValue {successful,               ///< The algorithm was successful in finding a root.
                      max_iterations_exceeded,  ///< The maximum number of iterations has bee exceeded without convergences.
                      spurious_convergence,     ///< The algorithm has converged to a local minima, i.e. the gradient is zero but the function is not.
                      line_search_error,        ///< Line search reported an error.
                      return_value_size         ///< Indicator value.
    };

    /// Shorthand for the templated line search algorithm
    typedef LineSearchAlgorithm<Scalar, Vector> LSA;

    /// The implementation of Newton root finding algorithm
    ///
    /// \param x_start The starting point of the Newton root finding algorithm.
    /// \param x_final The return value used if a root is found.
    /// \param function The function to find root for (functor).
    /// \param jacobian The Jacobian of the function.
    ///
    /// \return A flag indicating the whether the algorithm was sucessfull in finding a root.
    ///
    /// \tparam Function The type of the function to find root of (must be a functor)
    /// \tparam Jacobian The type of the jacobian (must be a functor)
    /// \tparam Derived The base type of starting vector.
    template <class Function,
              class Jacobian,
              typename Derived>
    ReturnValue newton(Eigen::MatrixBase<Derived>& x_start,
                       Vector& x_final,
                       Function function,
                       Jacobian jacobian) {

        // Make an identity vector with same size as x
        const Vector identity = Vector::Ones(x_start.size());

        // The return value of the algorithm
        ReturnValue return_value = max_iterations_exceeded;

        // Construct the error function and evaluate it
        ErrorFunction<Vector, Scalar, Function> error_function(function, x_start.size());
        error_function.template operator()<Vector>(x_start);

        // Test if the starting value is a root
        if ( error_function.get_function_value()->cwiseAbs().maxCoeff() < 0.01*tolerance_function ) {
            x_final = x_start;
            return_value = successful;
        }
        else {
            // Set the maximal step size based on the step factor and the value of x_start;
            const Scalar max_step_size = max_step_factor * std::max(x_start.norm(), static_cast<Scalar>(x_start.size()));

            // Set the initial value of x
            Vector x = x_start;

            for (unsigned int iteration=0; iteration < max_iterations; ++iteration) {
                // Calculate the Jacobian in x
                Array jacobian_value(x_start.size(), x_start.size());
                jacobian(x, *error_function.get_function_value(), jacobian_value);

                // Calculate the gradient of the error function
                Vector gradient = jacobian_value.transpose() * (*error_function.get_function_value());

                // Save the old value of x
                Vector x_old = x;
                Scalar error_old = *error_function.get_error_value();

                // Solve the linear equation
                //
                //    jacobian * delta = -function,
                //
                // in order to find the delta direction.
                Vector delta = jacobian_value.colPivHouseholderQr().solve(-(*error_function.get_function_value()));

                // Do the line search to find the "optimal" delta
                typename LSA::ReturnValue linesearch_return_value = line_search_algorithm.linesearch(x_old, error_old, gradient, delta, x, error_function, max_step_size);

                // Check if line search was successful
                if (linesearch_return_value > LSA::successful && linesearch_return_value != LSA::lambda_to_small) {
                    return_value = line_search_error;
                    break;
                }

                // Test for convergence in the function value using the max norm
                if (error_function.get_function_value()->cwiseAbs().maxCoeff() < tolerance_function) {
                    return_value = successful;
                    break;
                }

                // Test for spurious convergence
                if (linesearch_return_value == LSA::lambda_to_small) {
                    if( gradient.cwiseAbs().cwiseProduct(x.cwiseAbs().cwiseMax(identity) / std::max(*error_function.get_error_value(), 0.5*static_cast<Scalar>(x_start.size()))).maxCoeff() < tolerance_gradient ) {
                        return_value = spurious_convergence;
                    }
                    else {
                        return_value = successful;
                    }

                    break;
                }

                // Test for convergence in delta
                if ( (x_old-x).cwiseAbs().cwiseQuotient(x.cwiseAbs().cwiseMax(identity)).maxCoeff() < tolerance_x ) {
                    return_value = successful;
                    break;
                }
            }

            // Set x_final no matter how the loop was interrupted
            x_final = x;

        }

        return return_value;
    }

private:
    const Scalar tolerance_x;
    const Scalar tolerance_function;
    const Scalar tolerance_gradient;
    const Scalar max_step_factor;
    const unsigned int max_iterations;

    LSA line_search_algorithm;
};


} // namespace Newton
} // namespace Muninn

#endif /* MUNINN_NEWTON_NEWTONROOTFINDER_H_ */

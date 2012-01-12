// LineSearchAlgorithm.h
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

#ifndef MUNINN_NEWTON_LINESEARCH_ALGORITHM_H_
#define MUNINN_NEWTON_LINESEARCH_ALGORITHM_H_

#include <limits>

#include "Eigen/Core"

namespace Muninn {
namespace Newton {

/// Class with implements line search functionality: find a distance a long a
/// line where a given function is decreased sufficiently. The criterion for a
/// sufficient decreasement is that
/// \f[
///    f(x_\mathrm{new}) \leq f(x_\mathrm{old}) + \alpha * G(x_\mathrm{old}) * (x_\mathrm{new} - x_\mathrm{old})
/// \f]
/// where \f$ G \f$ is the gradient of the cost function \f$ f \f$. The
/// algorithm is described in "Numerical Recipes in C++".
///
/// \tparam Scalar The type used for scalars in the algorithm.
/// \tparam Vector The type used for vectors in the algorithm.
template <typename Scalar, typename Vector>
class LineSearchAlgorithm {
public:
    /// Constructor for the line search algorithm
    ///
    /// \param alpha Required average rate of decrease in f.
    /// \param tolerance_x The tolerance in x.
    LineSearchAlgorithm(const Scalar& alpha=1E-4,
                        const Scalar& tolerance_x = -1) :
        alpha(alpha),
        tolerance_x(tolerance_x>=0 ? tolerance_x : std::numeric_limits<Scalar>::epsilon()) {}

    /// Return values for line search algorithm
    enum ReturnValue {successful=0, lambda_to_small, function_encreasing_in_delta_direction, return_value_size};

    /// The implementation of the line search algorithm
    ///
    /// \param x_old The starting point of the line search
    /// \param function_x_old The function value in x_old
    /// \param gradient_x_old The gradient value in x_old
    /// \param delta The direction to search along
    /// \param x_new The returned new value of x
    /// \param function The cost function to minimize (functor)
    /// \param max_step_size The maximal step size
    ///
    /// \return A flag indicating the whether the search was successful.
    ///
    /// \tparam Function The type of the function to minimize (must be a functor)
    /// \tparam Derived1 The base type of x_old
    /// \tparam Derived2 The base type of gradient_x_old
    /// \tparam Derived3 The base type of delta
    /// \tparam Derived4 The base type of x_new
    template <class Function,
              typename Derived1,
              typename Derived2,
              typename Derived3,
              typename Derived4>
    ReturnValue linesearch(const Eigen::MatrixBase<Derived1>& x_old,
                           const Scalar& function_x_old,
                           const Eigen::MatrixBase<Derived2>& gradient_x_old,
                           Eigen::MatrixBase<Derived3>& delta,
                           Eigen::MatrixBase<Derived4>& x_new,
                           Function& function,
                           const Scalar max_step_size=100) {

        // Make an identity vector with same size as x
        Vector identity = x_old;
        identity.setOnes();

        // Check that the step length is not longer than max_step_size
        if (delta.norm() > max_step_size) {
            delta = delta * (max_step_size/delta.norm());
        }

        // Calculate g(0) and g'(0), where g(lambda) = f(x_old + lambda*delta)
        Scalar g_0 = function_x_old;
        Scalar g_prime_0 = gradient_x_old.dot(delta);

        // Check that the function is decreasing in the direction of delta
        if (! g_prime_0 < 0) {
            return function_encreasing_in_delta_direction;
        }

        // Calculate the minimal acceptable lambda value
        Scalar lambda_min = tolerance_x / delta.cwiseAbs().cwiseQuotient(x_old.cwiseAbs().cwiseMax(identity)).maxCoeff();

        // Set the initial lambda value
        Scalar lambda = 1.0;

        // This variable is set to true, when the convergence condition is fulfilled
        bool condition = false;

        // Loop invariant variables
        Scalar lambda_new;
        Scalar lambda1 = 0;
        Scalar lambda2 = 0;
        Scalar g_lambda1 = 0;
        Scalar g_lambda2 = 0;

        do {
            // Check that lambda is not to small
            if (lambda < lambda_min) {
                x_new = x_old;
                return lambda_to_small;
            }

            // Update the x value
            x_new = x_old + lambda * delta;
            Scalar function_new = function.template operator()<Vector>(x_new);

            // Check if the convergence condition is fulfilled
            condition = function_new <= function_x_old + alpha*lambda*g_prime_0;

            // If the convergence condition is not met, adjust lambda.
            // The new lambda value is stored in lambda_new
            if (!condition) {
                // Update the previous values of lambda and g
                lambda2 = lambda1;
                lambda1 = lambda;

                g_lambda2 = g_lambda1;
                g_lambda1 = function_new;

                // First time use a quadratic approximation of g.
                // In this case the minima is found by solving a linear equation;
                // which is the derivative of the approximation equated to zero.
                if (lambda > 1-std::numeric_limits<Scalar>::epsilon()) {
                    lambda_new = -g_prime_0 / (2 * (function_new - g_0 - g_prime_0) );
                }
                // Subsequently use a cubic approximation of g.
                // In this case the minima is found by solving a quadratic;
                // which is the derivative of the approximation equated to zero.
                else {
                    Scalar factor1 = (g_lambda1 - g_prime_0*lambda1 - g_0)/(lambda1*lambda1);
                    Scalar factor2 = (g_lambda2 - g_prime_0*lambda2 - g_0)/(lambda2*lambda2);

                    Scalar a = (factor1 - factor2)/(lambda1-lambda2);
                    Scalar b = (-lambda2*factor1 + lambda1*factor2)/(lambda1-lambda2);

                    if (a==0) {
                        // In this case the equation is linear.
                        lambda_new = -g_prime_0/(2*b);
                    }
                    else {
                        // Calculate the discriminant of the quadratic equation
                        // and check if it's positive.
                        Scalar discriminant = b*b - 3*a*g_prime_0;

                        if (discriminant >= 0) {
                            lambda_new = (-b + sqrt(discriminant))/(3*a);
                        }
                        else {
                            lambda_new = 0.5*lambda;
                        }
                    }

                    // When using the cubic approximation, lambda should always be at least halfed
                    lambda_new = std::min(lambda_new, 0.5*lambda);
                }

                // Lambda should never be smaller than 0.1 of the old value
                lambda = std::max(lambda_new, 0.1*lambda);
            }

        } while (!condition);

        return successful;
    }

private:
    const Scalar alpha;       ///< Required average rate of decrease in f
    const Scalar tolerance_x; ///< The tolerance in x
};


} // namespace Newton
} // namespace Muninn

#endif /* MUNINN_NEWTON_LINESEARCH_ALGORITHM_H_ */

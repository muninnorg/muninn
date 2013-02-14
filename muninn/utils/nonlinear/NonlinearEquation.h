// NonlinearEquation.h
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

#ifndef MUNINN_NONLINEAREQUATION_H_
#define MUNINN_NONLINEAREQUATION_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

namespace Muninn {

class NonlinearEquation {
public:

    /// Default destructor
    virtual ~NonlinearEquation() {}

    ///  The nonlinear function, where both X and F must be one dimensional of shape (n).
    ///
    /// \param X The argument, which must be one dimensional and of shape (n).
    /// \param F The return function value, which must be one dimensional and of shape (n).
    virtual void function(const DArray &X, DArray &F) = 0;

    /// The Jacobian of the vector function, where both X and F must be provided and have shape (n).
    ///
    /// \param X The argument, which must be one dimensional and of shape (n).
    /// \param F The function value corresponding to F, which must be one dimensional and of shape (n).
    /// \param J The return value of the Jacobian, which must have have shape (n,n).
    virtual void jacobian(const DArray &X, const DArray &F, DArray &J) = 0;
};

} // namespace Muninn

#endif /* MUNINN_NONLINEAREQUATION_H_ */

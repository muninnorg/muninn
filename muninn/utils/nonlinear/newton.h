// newton.h
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

#ifndef MUNINN_NEWTON_H_
#define MUNINN_NEWTON_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/nonlinear/NonlinearEquation.h"

namespace Muninn {

/// Finds a root in a system of 'n' nonlinear functions in 'n' variables by the
/// globally convergent Newton routine.
///
/// \param X Initial starting point and the return value that has shape (n)
/// \param eqn The nonlinear equations system of n equations
/// \return The return value of the Newton algorithm - 0 if successful.
int newton(DArray &X, NonlinearEquation &eqn);

} // namespace Muninn

#endif /* MUNINN_NEWTON_H_ */

// common.h
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

#ifndef MUNINN_COMMON_H_
#define MUNINN_COMMON_H_

#define MUNINN_MULTITHREADED

#include "muninn/Exceptions/MuninnException.h"
#include "muninn/Exceptions/MessageException.h"
#include "muninn/utils/MessageLogger.h"

/// \namespace Muninn The Muninn namespace.
/// All Muninn code is encapsulated in this namespace-

/// \namespace Muninn::nr The numerical recipes (nr) namespace.
/// All code in this namespace should be removed.

/// \namespace Muninn::MLEutils Namespace for utilities related to the
/// maximum likelihood estimator (MLE).

namespace Muninn {

// Forward deceleration of the TArray
template<typename T> class TArray;

/// The type used of representing counts in the histograms of Muninn.
/// This type must be able to hold the total number of counts in each iteration.
typedef unsigned long long Count;

/// The type used for a TArray containing counts.
typedef TArray<Count> CArray;

} // namespace Muninn

#endif /* MUNINN_COMMON_H_ */

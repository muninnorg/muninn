// Identity.h
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

#ifndef MUNINN_IDENTITY_H_
#define MUNINN_IDENTITY_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

namespace Muninn {
namespace MLEutils {

/// Identity functor, that converts a integer to a double. This functor can be
/// used instead of an explicit DArray with uniform binning.
class Identity {
public:
    /// Convert thin input value integer to double.
    ///
    /// \param value The input value.
    /// \return The input value as a double.
    inline double operator()(const unsigned int value) const {
        return static_cast<double>(value);
    }
};

} // namespace Muninn
} // namespace MLEutils

#endif // MUNINN_IDENTITY_H_

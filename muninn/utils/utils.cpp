// utils.cpp
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

#include <algorithm>
#include <cassert>

#include "muninn/utils/utils.h"

namespace Muninn {

std::vector<double> calculate_fractiles(std::vector<double> &values, const std::vector<double> &fractions) {
    assert(values.size()>0);

    std::vector<double> quantiles;
    std::sort(values.begin(), values.end());

    for (std::vector<double>::const_iterator it=fractions.begin(); it!=fractions.end(); it++) {
        assert(0.0<(*it) && (*it)<1.0);
        int quantile_pos = static_cast<int>( (*it)*values.size() );
        double quantile = values[quantile_pos];
        quantiles.push_back(quantile);
    }
    return quantiles;
}

} // namespace Muninn

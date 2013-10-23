// CGE.cpp
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

#include "muninn/CGE.h"
#include <algorithm>

namespace Muninn {

void CGE::estimate_new_weights(){
    if (initial_collection) {
        // Initialize the binner with the collected observations
        binner->initialize(initial_observations, initial_beta);
        unsigned int nbins = binner->get_nbins();

        // Now update the histogram with the new number of bins
        ge.extend(newvector(0u), newvector(nbins));

        // Set the weights for the current histogram
        DArray bin_centers = binner->get_binning_centered();
        DArray lnw(nbins);
        for (DArray::flatiterator it=lnw.get_flatiterator(); it(); ++it)
            *it = -initial_beta * bin_centers(it);

        ge.current = ge.estimator->new_histogram(lnw);

        // Add the observations to the histogram
        for(std::vector<double>::iterator it = initial_observations.begin(); it < initial_observations.end(); it++) {
            unsigned int bin = binner->calc_bin(*it);
            ge.add_observation(bin);
        }

        // And update the entropy estimate
        ge.estimate_new_weights(binner);

        // Update the collection state variable
        initial_collection = false;
    }
    else {
        ge.estimate_new_weights(binner);
    }
}

} // namespace Muninn

// MLEestimate.h
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

#ifndef MUNINN_MLEESTIMATE_H_
#define MUNINN_MLEESTIMATE_H_

#include <vector>
#include <map>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimate.h"
#include "muninn/utils/BaseConverter.h"

namespace Muninn {

class MLEestimate : public Estimate, public BaseConverter<Estimate, MLEestimate> {
public:
    MLEestimate(const DArray &lnG, const BArray &lnG_support, const std::vector<unsigned int> x0) :    Estimate(lnG, lnG_support, x0) {}
    MLEestimate(const std::vector<unsigned int> &nbins) : Estimate(nbins) {}

    virtual ~MLEestimate() {}

    friend class MLE;

private:
    std::map<const Histogram*, double> free_energies;   // The estimated free energies for each histogram
};

} // namespace Muninn

#endif /* MUNINN_MLEESTIMATE_H_ */

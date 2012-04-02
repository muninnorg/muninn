// Estimate.h
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

#ifndef MUNINN_ESTIMATE_H_
#define MUNINN_ESTIMATE_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/utils/utils.h"
#include "muninn/utils/StatisticsLogger.h"

namespace Muninn {

/// This class contains the information related to an estimate of the entropy
/// (lnG). An the Estimator::estimate() function will take an old estimate as
/// an argument and update this estimate based on the current history and
/// weights.
class Estimate : public Loggable {
public:
    /// Constructor using values of the entropy, support and reference bin.
    ///
    /// \param lnG The estimated entropy.
    /// \param lnG_support The support for the estimated entropy.
    /// \param x0 The reference bin used for the estimate. The estimated
    ///           entropy should have a fixed reference value in this bin.
    Estimate(const DArray &lnG, const BArray &lnG_support, const std::vector<Index> x0) :
        lnG(lnG), lnG_support(lnG_support), x0(x0), shape(lnG.get_shape()) {
        assert(lnG.same_shape(lnG_support));
        assert(x0.size()==0 || x0.size()==lnG.get_ndims());
        assert(lnG.valid_coord(x0));                           // Note that an empty vector also is a valid coordinate
    }

    /// Construct an empty estimate with a given shape.
    ///
    /// \param shape The shape of the empty estimate.
    Estimate(const std::vector<Index> &shape) : lnG(shape), lnG_support(shape), shape(shape) {}

    /// Empty virtual destructor
    virtual ~Estimate() {}

    // Getters

    /// Getter for the estimated entropy (lnG)
    ///
    /// \return The estimated entropy.
    inline const DArray& get_lnG() const {return lnG;}

    /// Getter for the support corresponding to the estimated entropy
    ///
    /// \return The support.
    inline const BArray& get_lnG_support() const {return lnG_support;}

    /// Getter for the reference bin
    ///
    /// \return The reference bin.
    inline const std::vector<Index>& get_x0() const {return x0;}

    /// Get the shape of the estimate.
    ///
    /// \return The shape of the estimate.
    inline const std::vector<Index>& get_shape() const {return shape;}

    // Setters

    /// Setter for the entropy (lnG).
    ///
    /// \param new_lnG An array with the new entropy estimate. The array
    ///                must have the same shape as the current estimate.
    inline void set_lnG(const DArray &new_lnG) {
        assert(new_lnG.has_shape(shape));
        lnG = new_lnG;
    }

    /// Setter for the entropy (lnG).
    ///
    /// \param new_lnG All entries in the entropy will get this value.
    inline void set_lnG(double new_lnG) {
        lnG = new_lnG;
    }

    /// Setter for the support of the estimated entropy.
    ///
    /// \param new_lnG_support The new support.
    inline void set_lnG_support(const BArray &new_lnG_support) {
        assert(new_lnG_support.has_shape(shape));
        lnG_support = new_lnG_support;
    }

    /// Setter for the support of the estimated entropy.
    ///
    /// \param new_lnG_support All entries in the support will get this value.
    inline void set_lnG_support(bool new_lnG_support) {
        lnG_support = new_lnG_support;
    }

    /// Setter for the reference bin.
    /// \param new_x0 New value for the refernce bin.
    inline void set_x0(const std::vector<Index> &new_x0) {
        assert(new_x0.size() == 0 || new_x0.size() == shape.size());
        x0 = new_x0;
    }

    /// Method for extending the shape of the Estimate. This method should
    /// normally be called through Estimator::extend_estimate().
    ///
    /// \param add_under The number of bins to be added leftmost in all dimensions.
    /// \param add_over The number of bins to be added rightmost in all dimensions.
    virtual void extend(const std::vector<Index> &add_under, const std::vector<Index> &add_over) {
        lnG = lnG.extended(add_under, add_over);
        lnG_support = lnG_support.extended(add_under, add_over);
        if (x0.size()>0)
            x0 = add_vectors(add_under, x0);
        shape = lnG.get_shape();
    }

    /// Add an entries to the statistics log. This function implements the
    /// Loggable interface.
    ///
    /// \param statistics_logger The logger to add an entry to.
    virtual void add_statistics_to_log(StatisticsLogger& statistics_logger) const {
        statistics_logger.add_entry("lnG", lnG);
        statistics_logger.add_entry("lnG_support", lnG_support);
        statistics_logger.add_entry("x_zero", vector_to_TArray<Index>(x0));
    }

private:
    DArray lnG;                ///< The estimated entropy.
    BArray lnG_support;        ///< The support for the estimate of the entropy.
    std::vector<Index> x0;     ///< The index of the reference bin for the entropy (the entropy has a fixed value in this bin).
    std::vector<Index> shape;  ///< The shape of the estimate.
};

} // namespace Muninn

#endif /* MUNINN_ESTIMATE_H_ */

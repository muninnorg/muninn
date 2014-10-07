// CGEcollection.h
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

#ifndef MUNINN_CGE_COLLECTION_H_
#define MUNINN_CGE_COLLECTION_H_

// TODO: Add documentation
#include <cmath>

#include "muninn/CGE.h"
#include "muninn/Binners/NonUniformDynamicBinner.h"
#include "muninn/utils/polation/SupportBoundaries.h"
#include "muninn/utils/polation/AverageSlope.h"

namespace Muninn {

class CGEcollection {
public:

    CGEcollection() {}

    ~CGEcollection() {
        for (size_t i=0; i<cge_objects.size(); ++i) {
            if (ownership.at(i)) {
                delete cge_objects.at(i);
            }
        }
    }

    void add_cge(CGE* cge, bool pass_ownership=false) {
        cge_objects.push_back(cge);
        ownership.push_back(pass_ownership);
    }

    void unify_binners_range() {
        MessageLogger::get().info("Unifying the range of the binners.");

        std::vector<double> all_initial_observations;

        if (cge_objects.front()->initial_collection) {
            // Collect all the initial observations
            for (size_t i=0; i<cge_objects.size(); ++i) {
                all_initial_observations.insert(all_initial_observations.end(), cge_objects.at(i)->initial_observations.begin(), cge_objects.at(i)->initial_observations.end());
            }

            // Initialize all the binners with the same samples
            for (size_t i=0; i<cge_objects.size(); ++i) {
                cge_objects.at(i)->get_binner().initialize(all_initial_observations);
            }
        }
        else {
            // Find the common min and max value for binning
            double min_value = std::numeric_limits<double>::infinity();
            double max_value = -std::numeric_limits<double>::infinity();

            for (size_t i=0; i<cge_objects.size(); ++i) {
                NonUniformDynamicBinner& binner = NonUniformDynamicBinner::cast_from_base(cge_objects.at(i)->get_binner(), "The CGEcollection is only compatible with CGE classes that uses a NonUniformDynamicBinner.");
                DArray centered = binner.get_binning_centered();
                min_value = std::min(min_value, centered(0));
                max_value = std::max(max_value, centered(centered.get_asize()-1));
            }

            // Extend all the binners to include the min and max value
            for (size_t i=0; i<cge_objects.size(); ++i) {
                CGE* cge = cge_objects.at(i);
                NonUniformDynamicBinner& binner = NonUniformDynamicBinner::cast_from_base(cge->get_binner(), "The CGEcollection is only compatible with CGE classes that uses a NonUniformDynamicBinner.");

                std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension_left = binner.include(min_value, cge->get_ge().get_estimate(), cge->get_ge().get_history(), cge->get_ge().get_current_histogram().get_lnw());
                cge->ge.extend(extension_left.first, extension_left.second, &binner);

                std::pair<std::vector<unsigned int>, std::vector<unsigned int> > extension_right = binner.include(max_value, cge->get_ge().get_estimate(), cge->get_ge().get_history(), cge->get_ge().get_current_histogram().get_lnw());
                cge->ge.extend(extension_right.first, extension_right.second, &binner);

            }
        }
    }

    void unify_binners_extension() {
        MessageLogger::get().info("Unifying the extension protocol of the binners.");

        // Find the CGE object that has explored the lowest and highest values
        Index min_support_bin = std::numeric_limits<Index>::max();
        Index max_support_bin = std::numeric_limits<Index>::min();

        CGE* cge_left = NULL;
        CGE* cge_right = NULL;

        for (size_t i=0; i<cge_objects.size(); ++i) {
            CGE* cge = cge_objects.at(i);
            const BArray& lnG_support = cge->get_ge().get_estimate().get_lnG_support();

            Index left_bound = MLEutils::SupportBoundaries1D::find_left_bound(lnG_support);

            if (left_bound < min_support_bin) {
                min_support_bin = left_bound;
                cge_left = cge;
            }

            Index right_bound = MLEutils::SupportBoundaries1D::find_right_bound(lnG_support);
            if (right_bound > max_support_bin) {
                max_support_bin = right_bound;
                cge_right = cge;
            }
        }

        // Calculate the left and right slope for these objects
        const MultiHistogramHistory& history_left = MultiHistogramHistory::cast_from_base(cge_left->get_ge().get_history(), "The CGEcollection is only compatible with the MultiHistogramHistory.");
        NonUniformDynamicBinner& binner_left = NonUniformDynamicBinner::cast_from_base(cge_left->get_binner(), "The CGEcollection is only compatible with CGE classes that uses a NonUniformDynamicBinner.");
        double slope_left = MLEutils::AverageSlope1d<DArray>::get_slope(min_support_bin,
                cge_left->get_ge().get_current_histogram().get_lnw(),
                cge_left->get_ge().get_estimate().get_lnG_support(),
                history_left.get_sum_N(),
                binner_left.get_binning_centered(),
                binner_left.get_sigma());

        const MultiHistogramHistory& history_right = MultiHistogramHistory::cast_from_base(cge_right->get_ge().get_history(), "The CGEcollection is only compatible with the MultiHistogramHistory.");
        NonUniformDynamicBinner& binner_right = NonUniformDynamicBinner::cast_from_base(cge_right->get_binner(), "The CGEcollection is only compatible with CGE classes that uses a NonUniformDynamicBinner.");
        double slope_right = MLEutils::AverageSlope1d<DArray>::get_slope(max_support_bin,
                cge_right->get_ge().get_current_histogram().get_lnw(),
                cge_right->get_ge().get_estimate().get_lnG_support(),
                history_right.get_sum_N(),
                binner_right.get_binning_centered(),
                binner_right.get_sigma());

        // Set this slope in all binner objects
        for (size_t i=0; i<cge_objects.size(); ++i) {
            NonUniformDynamicBinner& binner = NonUniformDynamicBinner::cast_from_base(cge_objects.at(i)->get_binner(), "The CGEcollection is only compatible with CGE classes that uses a NonUniformDynamicBinner.");
            binner.set_slopes(slope_left, slope_right);
        }
    }

    CGE& at(size_t index) {
        return *cge_objects.at(index);
    }

    bool check_consistent_binning() {
        DArray reference = cge_objects.front()->get_binning();

        for (size_t i=0; i<cge_objects.size(); ++i) {
            DArray binning = cge_objects.at(i)->get_binning();

            if(!reference.same_shape(binning)) {
                MessageLogger::get().debug("Mismatch in shape.");
                return false;
            }

            for (Index index=0; index<reference.get_asize(); ++index) {
                if (abs(reference(index)-binning(index)) > 1E-6) {
                    MessageLogger::get().debug("Mismatch in binning.");
                    return false;
                }
            }
        }

        return true;
    }

private:
    std::vector<CGE*> cge_objects;
    std::vector<bool> ownership;
};

} // namespace Muninn

#endif // MUNINN_CGE_COLLECTION_H_

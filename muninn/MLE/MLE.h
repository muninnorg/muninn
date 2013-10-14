// MLE.h
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

#ifndef MUNINN_MLE_H_
#define MUNINN_MLE_H_

#include <deque>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/Estimator.h"
#include "muninn/Histories/MultiHistogramHistory.h"
#include "muninn/MLE/MLEestimate.h"

namespace Muninn {

/// The maximum likelihood estimator (MLE)s. This estimator uses the
/// generalized multihistogram (GMH) equations for estimating the entropy.
class MLE: public Estimator, public BaseConverter<Estimator, MLE> {
public:
    /// Constructor for the MLE class.
    ///
    /// \param min_count The minimal number of counts in a bin in order to have
    ///                  support in the given bin.
    /// \param memory The maximal number of histogram to keep in the history.
    /// \param restricted_individual_support Restrict the support of the individual histograms to only
    ///                                      cover the support for the individual histogram.
    /// \param history_mode Describes the procedure for deleting old histograms.
    /// \param sigma The number of bins used in the Gaussian kernel, when printing beta values.
    MLE(Count min_count=30, unsigned int memory=20, bool restricted_individual_support=false,
        MultiHistogramHistory::HistoryMode history_mode=MultiHistogramHistory::DROP_OLDEST,
        unsigned int sigma=20) :
            min_count(min_count), memory(memory), restricted_individual_support(restricted_individual_support),
            history_mode(history_mode), sigma(sigma) {}

    virtual ~MLE() {}

    // Implementation of Estimator interface (see base class for documentation).
    virtual void estimate(const History &history, Estimate &estimate, const Binner *binner=NULL);

    // Implementation of Estimator interface (see base class for documentation).
    virtual void extend_estimate(const History &extended_history, Estimate &estimate, const std::vector<unsigned int> &add_under, const std::vector<unsigned int> &add_over);

    // Implementation of Estimator interface (see base class for documentation).
    virtual Histogram* new_histogram(const std::vector<unsigned int> &shape) {
        return new Histogram(shape);
    }

    // Implementation of Estimator interface (see base class for documentation).
    virtual Histogram* new_histogram(const DArray &lnw) {
        return new Histogram(lnw);
    }

    // Implementation of Estimator interface (see base class for documentation).

    // Implementation of Estimator interface (see base class for documentation).
    virtual History* new_history(const std::vector<unsigned int> &shape) {
        return new MultiHistogramHistory(shape, memory, min_count, history_mode);
    }

    // Implementation of Estimator interface (see base class for documentation).
    virtual Estimate* new_estimate(const std::vector<unsigned int> &shape) {
        return new MLEestimate(shape);
    }

    /// Make a new MLEestimate based on values of lnG, lnG support, a reference
    /// bin, free energies and a given history. The Estimate is compatible with
    /// the MLE estimator.
    ///
    /// \param lnG The entropy to construct the estimate from.
    /// \param lnG_support The support for the estimated entropy.
    /// \param x0 There reference bin the estimate is based on.
    /// \param free_energies The estimated free energies for the histograms in
    ///                      the history. The energies should be in the same
    ///                      order as the histograms has in the history.
    /// \param base_history The history to make the estimate from.
    /// \param binner If a Binner has been use, it can be passed to the estimator.
    /// \return A new empty Estimate.
    virtual Estimate* new_estimate(const DArray &lnG, const BArray &lnG_support, const std::vector<unsigned int> &x0, const DArray &free_energies, const History &base_history, const Binner *binner=NULL);

private:
    const Count min_count;                           ///< The minimal number of counts in a bin in order to have support in a bin.
    const unsigned int memory;                       ///< The maximal number of histogram to keep in the history.
    bool restricted_individual_support;              ///< Restrict the support of the individual histograms to only cover the support for the individual histogram.
    MultiHistogramHistory::HistoryMode history_mode; ///< Describes the procedure for deleting old histograms.
    unsigned int sigma;                              ///< The number of bins used in the Gaussian kernel, used when printing beta values

    /// Give an initial guess of the free energy (-ln(Z)) for the first (newest)
    /// histogram in the history (history[0]) using the previous estimated
    /// entropy (lnG), as described in equation (A.4) in [JFB02].
    ///
    /// \param history The history to base the estimate on.
    /// \param lnG The previous estimate of the entropy to base the initial
    ///            guess of the free energy on.
    /// \param sum_N The sum histogram.
    /// \param support_n The number of counts in each histogram, but only
    ///                  summed over the bins with support.
    /// \param x0 The index of the reference bin for the entropy (the entropy
    ///           has a fixed value in this bin).
    /// \return A guess of the free energy for the first histogram.
    double initial_free_energy_estimate(const MultiHistogramHistory &history, const DArray &lnG, const CArray &sum_N, const CArray &support_n, const std::vector<Index> x0);

    /// Calculate a estimate of the entropies from the estimated free energies,
    /// as described in equation (4.2) in [JFB02].
    ///
    /// \param history The history to base the estimate on.
    /// \param sum_N The sum histogram.
    /// \param support The support history.
    /// \param support_n The number of counts in each histogram, but only
    ///                  summed over the bins with support.
    /// \param free_energy The estimated free energies for the individual
    ///                    histograms in the history.
    /// \param new_lnG The new calculated/estimated entropy (note that the
    ///                array must have the corrected size).
    static void calc_lnG(const MultiHistogramHistory &history, const CArray &sum_N, const BArray &support, const CArray &support_n, const DArray &free_energy, DArray &new_lnG);

    /// Calculate a estimate of the entropies from the estimated free energies
    /// as calc_lnG, but with the local support replaced by the accumulated
    /// support.
    ///
    /// \param history The history to base the estimate on.
    /// \param sum_N The sum histogram.
    /// \param accumulated_N The i'th array in this vector contains the sum
    ///                      histogram for the first i histograms.
    /// \param support The support history.
    /// \param support_n The number of counts in each histogram, but only
    ///                  summed over the bins with support.
    /// \param free_energy The estimated free energies for the individual
    ///                    histograms in the history.
    /// \param new_lnG The new calculated/estimated entropy (note that the
    ///                array must have the corrected size).
    static void calc_lnG_accumulated(const MultiHistogramHistory &history, const CArray &sum_N, const std::vector<CArray> &accumulated_N, const BArray &support, const CArray &support_n, const DArray &free_energy, DArray &new_lnG);
};

/// Exception to be thrown if the history contains non-overlapping histograms.
class MLENoOverlapException : public EstimatorException {
    virtual const char* what() const throw() {
        return "MLE estimation failed due to non overlapping histograms.";
    }
};

/// Exception to be thrown if the MLE could not find a solution to the
/// generalized multihistogram (GMH) equations.
class MLENoSolutionException : public EstimatorException {
    virtual const char* what() const throw() {
        return "MLE estimation failed to find a free energy solution to the GMH equations.";
    }
};

} // namespace Muninn

#endif // MUNINN_MLE_H_

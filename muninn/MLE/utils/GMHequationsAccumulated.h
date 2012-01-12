// GMHequationsAccumulated.h
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

#ifndef MUNINN_GMHEQUATIONSACCUMULATED_H_
#define MUNINN_GMHEQUATIONSACCUMULATED_H_

#include <limits>
#include <vector>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"
#include "muninn/utils/TArrayUtils.h"
#include "muninn/utils/TArrayMath.h"
#include "muninn/utils/nonlinear/NonlinearEquation.h"

namespace Muninn {


/// This class implements the GMH equations. as described in section A.1.3 in
/// [JFB02]. However in this implementation, the local support is replaced by
/// an accumulated support.
///
/// See the class GMHequations for further details on the GMHequaions.
class GMHequationsAccumulated: public NonlinearEquation {
public:

    /// Constructor for the GMH equations class with accumulated support.
    ///
    /// \param history The history the equations are based on.
    /// \param sum_N The sum histogram for the history (the sum of counts in
    ///               each bin).
    /// \param accumulated_N The accumulated number of counts in each bin.
    ///                      The value of accumulated_N[i](j) is the sum
    ///                      of counts in the bin with index j in the first i
    ///                      histograms.
    /// \param support The support of the history; support(j) is true if the
    ///                bin with index j has support.
    /// \param support_n The total number of observations in the individual
    ///                  histogram, but only summed over the bins with support.
    ///                  Accordingly, support_n(i) is the sum of the observations
    ///                  in all bins with support in the i'th histogram of the
    ///                  history.
    /// \param x0 This is the reference bin, the entropy in this bin is fixed
    ///           to lnG_x0.
    /// \param lnG_x0 The reference entropy in the reference bin x0.
    GMHequationsAccumulated(const MultiHistogramHistory &history, const CArray &sum_N, const std::vector<CArray> &accumulated_N, const BArray &support, const CArray &support_n, const std::vector<unsigned int> x0, const double &lnG_x0) :
        history(history), accumulated_N(accumulated_N), support(support), support_n(support_n), x0(x0), lnG_x0(lnG_x0), lnD(history.get_shape()) {
        ln_sum_N = TArray_log<DArray>(sum_N);
        this->support(x0) = false;
    }

    /// Default virtual destructor.
    virtual ~GMHequationsAccumulated() {};

    /// Calculate ln(D), where D is a function of the free energy and given
    /// by equation (A.9) in [JFB02] where it is denoted G.
    ///
    /// The result is stored in the private variable GMHequations::lnD and is
    /// used by the other function.
    ///
    /// \param free_energy The free energy to calculate D from. The array must
    ///                   have shape n, where n=history.get_size().
    void calc_lnD(const DArray &free_energy) {
        DArray summands(history.get_size());

        for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it) {
            // Calculate log of the terms in the sum given by equation (A.9) if [JFB02].
            for (unsigned int i=0; i<history.get_size(); i++) {
                if (accumulated_N[i](it) > 0)
                    summands(i) = log(support_n(i)) + history[i].get_lnw()(it) + free_energy(i);
                else
                    summands(i) = -std::numeric_limits<double>::infinity();
            }
            lnD(it) = log_sum_exp(summands);
        }
    }

    /// Calculates the spectral free energy \f$\vec{F}\f$, as given by equation
    /// (A.10) on page 118 in [JFB02].
    ///
    /// Note, that the function assumes that GMHequations::lnD is set correct.
    /// This means that the function GMHequations::calc_lnD should have been
    /// called previously with the same value of the free_energy.
    ///
    /// \param free_energy The free energy, which the spectral free energy is a
    ///                    function of. The array must have shape n, where
    ///                    n=history.get_size().
    /// \param F The resulting calculated spectral free energy (output). The
    ///          array must have shape n, where n=history.get_size().
    void spectral_free(const DArray &free_energy, DArray &F) {
        DArray summands(history.get_shape());

        for (unsigned int i=0; i<history.get_size(); i++) {
            summands = -std::numeric_limits<double>::infinity();

            for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it)
                if (accumulated_N[i](it) > 0)
                    summands(it) = history[i].get_lnw()(it) + ln_sum_N(it) - lnD(it);

            F(i) = -1 + exp(free_energy(i) + log_sum_exp(summands));

            if (accumulated_N[i](x0) > 0 )
                F(i) += exp(free_energy(i) + history[i].get_lnw()(x0) + lnG_x0);
        }
    }

    /// Calculates the jacobian of spectral free energy, as given by equation
    /// (A.10) on page 118 in [JFB02].
    ///
    /// Note, that the function assumes that GMHequations::lnD is set correct.
    /// This means that the function GMHequations::calc_lnD should have been
    /// called previously with the same value of the free_energy.
    ///
    /// \param free_energy The free energy, which Jacobian is a function of. The
    ///                    array must have shape n, where n=history.get_size().
    /// \param F The spectral free energy corresponding to the value of
    ///          free_energy. The array must have shape n, where
    ///          n=history.get_size().
    /// \param H The resulting Jacobian Matrix The spectral free energy
    ///          corresponding to the value of free_energy. The array must have
    ///          shape (n,n), where n=history.get_size().
    void spectral_free_jacobian(const DArray &free_energy, const DArray &F,    DArray &H) {
        DArray summands(history.get_shape());

        // Calculate H(i,j) for the upper triangle
        for (unsigned int i=0; i<free_energy.get_asize(); i++) {
            for (unsigned int j=i; j<free_energy.get_asize(); j++) {
                summands = -std::numeric_limits<double>::infinity();
                for (BArray::constwheretrueiterator it = support.get_constwheretrueiterator(); it(); ++it)
                    if (accumulated_N[i](it) > 0 && accumulated_N[j](it) > 0)
                        summands(it) = history[i].get_lnw()(it) + history[j].get_lnw()(it) + ln_sum_N(it) - 2*lnD(it);

                H(i,j) = -static_cast<double>(support_n(j)) * exp(free_energy(i) + free_energy(j) + log_sum_exp(summands));

                if (i==j)
                    H(i,j) += F(i) + 1.0;
            }
        }

        // Fill lower triangle, using H(i,j) = H(j,i) * n[j]/n[i] (see page 119 in [JFB02])
        for (unsigned int i=0; i<free_energy.get_asize(); i++) {
            for (unsigned int j=0; j<i; j++) {
                H(i,j) = H(j,i) * static_cast<double>(support_n(j)) / static_cast<double>(support_n(i));
            }
        }
    }

    /// Implementation of the NonlinearEquation interface.
    ///
    /// Let n=history.get_size() in the parameters.
    ///
    /// \param X The free energy, which must have shape (n).
    /// \param F The resulting spectral free energy, which must have shape (n).
    virtual void function(const DArray &X, DArray &F) {
        calc_lnD(X);
        spectral_free(X, F);
    }

    /// Implementation of the NonlinearEquation interface.
    ///
    /// \param X The free energy, which must have shape (n).
    /// \param F The spectral free energy corresponding to X, which must have shape (n).
    /// \param J The resulting jacobian, which must have shape (n,n).
    virtual void jacobian(const DArray &X, const DArray &F, DArray &J) {
        calc_lnD(X);
        spectral_free_jacobian(X, F, J);
    }

private:
    const MultiHistogramHistory &history;      ///< The history the equations are based on.
    const std::vector<CArray> &accumulated_N;  ///< The accumulated number of counts in each bin, for each histogram. The value of accumulated_N[i](j) is the sum of counts in the bin with index j in the first i histograms.
    BArray support;                            ///< The support of the history; support(j) is true if the bin with index j has support.
    const CArray &support_n;                   ///< The total number of observations in the individual histogram, but only summed over the bins with support.

    std::vector<unsigned int> x0;              ///< The index of the reference bin, where the entropy is fixed to lnG_x0.
    double lnG_x0;                             ///< The reference entropy in the reference bin x0.

    DArray ln_sum_N;                           ///< The log of the sum histogram for the history.
    DArray lnD;                                ///< ln(D), where D is given by equation (A.9) in [JFB02] where it is denoted G.
};

} // namespace Muninn

#endif /* GMHEQUATIONSACCUMULATED_H_ */

// NormalSampler.h
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

#ifndef MUNINN_NORMALSAMPLER_H_
#define MUNINN_NORMALSAMPLER_H_

#include <cmath>
#include <cstdlib>

/// Class for generating correlated samples from a capped normal distribution.
class NormalSampler {
public:

    /// Constructor
    ///
    /// \param mean The mean of the normal distribution.
    /// \param sigma The variance of the normal distribution.
    /// \param max_factor The minimal and maximal value for samples
    ///                   are -max_factor*sigma and max_factor*sigma.
    NormalSampler(double mean, double sigma, double max_factor=5.0) :
        mean(mean), sigma(sigma), value(mean), old_state(mean), can_undo(false), max_value(max_factor*sigma) {
        sigmasq = sq(sigma);
    }

    /// The function resets the sample to start at the mean of the distribution.
    void sample_first() {
        can_undo = false;
        value = mean;
    }

    /// Draw a sample from the proposal distribution.
    void sample() {
        can_undo = true;
        old_state = value;
        value += -sigma/10 + drandom() * sigma/5;
    }

    /// Step one sample back. This is used for rejecting the last sample.
    void step_one_back() {
        assert(can_undo);
        can_undo = false;
        value = old_state;
    }

    /// Get the energy/value of the latest sampled value.
    ///
    /// \return The energy of the latest sample.
    double energy() {
        return value;
    }

    /// Calculate the log-bias associated with the last sampling step.
    ///
    /// \return The bias.
    double log_bias() {
        assert(can_undo);

        if (mean-max_value < value && value < mean+max_value) {
            return -sq(value-mean)/(2*sigmasq) +sq(old_state-mean)/(2*sigmasq);
        }
        else
            return -std::numeric_limits<double>::infinity();
    }

private:
    double mean;        ///< The mean value for the normal distribution.
    double sigma;       ///< The variance of the normal distribution.
    double sigmasq;     ///< The square variance of the normal distribution.
    double value;       ///< The latest sampled value.
    double old_state;   ///< The previous sampled value.
    bool can_undo;      ///< Whether a previous value been sampled.
    double max_value;   ///< Then maximal value allowed (and -max_value is the minimal value).

    /// Function for generating a random number between 0 and 1.
    ///
    /// \return A random number between 0 and 1.
    double inline drandom() {
        return 1.0 * rand() / RAND_MAX;
    }

    /// Function for calculating the square of a double.
    ///
    /// \param v The value to square.
    /// \return The square of v.
    double inline sq(double v) {
        return v*v;
    }
};

#endif /* MUNINN_NORMALSAMPLER_H_ */

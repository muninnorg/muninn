// CGEfactory.h
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

#ifndef MUNINN_CGEFACTORY_H_
#define MUNINN_CGEFACTORY_H_

#include "muninn/common.h"
#include "muninn/CGE.h"
#include "muninn/Factories/CGEfactorySettingsException.h"

namespace Muninn {

/// Define the different generalized ensembles (GE).
enum GeEnum {GE_MULTICANONICAL=0, GE_INV_K=1, GE_ENUM_SIZE};

/// Define the string names corresponding values of the GeEnum.
static const std::string GeEnumNames[] = {"multicanonical", "invk"};

/// Input operator of a GeEnum from string.
std::istream &operator>>(std::istream &input, GeEnum &g);

/// Output operator for a GeEnum.
std::ostream &operator<<(std::ostream &o, const GeEnum &g);

/// A factory for creating a CGE object based on a local settings object.
/// The factory function creates a new CGE class, and all additional classes will be
/// created automatically and transparently.
class CGEfactory {
public:
    /// Local settings class
    class Settings {
    public:
        /// Weight-scheme to use: invk|multicanonical
        GeEnum weight_scheme;

        /// Slope factor used for the linear extrapolation of the weights, when the weights are increasing in the direction away from the main area of support.
        double slope_factor_up;

        /// Slope factor used for the linear extrapolation of the weights, when the weights are decreasing in the direction away from the main area of support.
        double slope_factor_down;

        /// The minimal beta value to be used based on thermodynamics and in the extrapolation.
        double min_beta;

        /// The maximal beta value to be used based on thermodynamics.
        double max_beta;

        /// The initial beta value to be used.
        double initial_beta;

        /// The resolution for the non-uniform binner. The binning is set so
        /// that a uniform resolution, \f$ r \f$, in the weights
        /// is obtained, \f$ |ln w(E_j) - \ln w(E_{j+1})| \simeq r \f$.
        double resolution;

        /// Use the initial bin with as maximal bin width, when expanding to the left.
        bool initial_width_is_max_left;

        /// Use the initial bin with as maximal bin width, when expanding to the right.
        bool initial_width_is_max_right;

        /// The filename for the Muninn statistics logfile.
        std::string statistics_log_filename;

        /// Muninn log mode (current|all). See Muninn::StatisticsLogger::Mode
        /// for details.
        Muninn::StatisticsLogger::Mode log_mode;

        /// Number of iterations used in first round of sampling.
        unsigned int initial_max;

        /// The number of consecutive histograms to keep in memory.
        unsigned int memory;

        /// The minimal number of counts in a bin in order to have support in that bin.
        unsigned int min_count;

        /// Restrict the support of the individual histograms to only cover the support for the given histogram.
        bool restricted_individual_support;

        /// Use dynamic binning.
        bool use_dynamic_binning;

        /// Bin width used for non-dynamic binning.
        double bin_width;

        /// The verbose level of Muninn.
        int verbose;

        /// Constructor that sets the default values for the settings.
        ///
        /// \param weight_scheme See documentation for Settings::weight_scheme.
        /// \param slope_factor_up See documentation for Settings::slope_factor_up.
        /// \param slope_factor_down See documentation for Settings::slope_factor_down
        /// \param min_beta See documentation for Settings::min_beta.
        /// \param max_beta See documentation for Settings::max_beta.
        /// \param initial_beta See documentation for Settings::initial_beta.
        /// \param resolution See documentation for Settings::resolution.
        /// \param initial_width_is_max_left See documentation for Settings::initial_width_is_max_left.
        /// \param initial_width_is_max_right See documentation for Settings::initial_width_is_max_right.
        /// \param statistics_log_filename See documentation for Settings::statistics_log_filename.
        /// \param log_mode See documentation for Settings::log_mode.
        /// \param initial_max  See documentation for Settings::initial_max.
        /// \param memory See documentation for Settings::memory.
        /// \param min_count See documentation for Settings::min_count.
        /// \param restricted_individual_support See documentation for Settings::restricted_individual_support.
        /// \param use_dynamic_binning See documentation for Settings::use_dynamic_binning.
        /// \param bin_width See documentation for Settings::bin_width.
        /// \param verbose See documentation for Settings::verbose.
        Settings(GeEnum weight_scheme=GE_MULTICANONICAL,
                 double slope_factor_up = 0.3,
                 double slope_factor_down = 3.0,
                 double min_beta=-std::numeric_limits<double>::infinity(),
                 double max_beta=std::numeric_limits<double>::infinity(),
                 double initial_beta=0.0,
                 double resolution = 0.2,
                 bool initial_width_is_max_left=true,
                 bool initial_width_is_max_right=false,
                 std::string statistics_log_filename = "muninn.txt",
                 Muninn::StatisticsLogger::Mode log_mode = Muninn::StatisticsLogger::ALL,
                 unsigned int initial_max = 5000,
                 unsigned int memory = 40,
                 unsigned int min_count = 30,
                 bool restricted_individual_support=false,
                 bool use_dynamic_binning=true,
                 double bin_width = 0.1,
                 int verbose=3)
        : weight_scheme(weight_scheme),
          slope_factor_up(slope_factor_up),
          slope_factor_down(slope_factor_down),
          min_beta(min_beta),
          max_beta(max_beta),
          initial_beta(initial_beta),
          resolution(resolution),
          initial_width_is_max_left(initial_width_is_max_left),
          initial_width_is_max_right(initial_width_is_max_right),
          statistics_log_filename(statistics_log_filename),
          log_mode(log_mode),
          initial_max(initial_max),
          memory(memory),
          min_count(min_count),
          restricted_individual_support(restricted_individual_support),
          use_dynamic_binning(use_dynamic_binning),
          bin_width(bin_width),
          verbose(verbose) {}

        /// Output operator
        friend std::ostream &operator<<(std::ostream &o, const Settings &settings) {
            o << "weight_scheme:" << settings.weight_scheme << std::endl;
            o << "slope_factor_up:" << settings.slope_factor_up << std::endl;
            o << "slope_factor_down:" << settings.slope_factor_down << std::endl;
            o << "min_beta:" << settings.min_beta << std::endl;
            o << "max_beta:" << settings.max_beta << std::endl;
            o << "initial_beta:" << settings.initial_beta << std::endl;
            o << "resolution:" << settings.resolution << std::endl;
            o << "initial_width_is_max_left:" << settings.initial_width_is_max_left << std::endl;
            o << "initial_width_is_max_right:" << settings.initial_width_is_max_right << std::endl;
            o << "statistics_log_filename:" << settings.statistics_log_filename << std::endl;
            o << "log_mode:" << Muninn::StatisticsLogger::ModeNames[settings.log_mode] << std::endl;
            o << "initial_max:" << settings.initial_max << std::endl;
            o << "memory:" << settings.memory << std::endl;
            o << "min_count:" << settings.min_count << std::endl;
            o << "restricted_individual_support:" << settings.restricted_individual_support << std::endl;
            o << "use_dynamic_binning:" << settings.use_dynamic_binning << std::endl;
            o << "bin_width:" << settings.bin_width << std::endl;
            o << "verbose:" << settings.verbose << std::endl;
            return o;
        }
    };

    /// Function for creating a new CGE class based on a setting object.
    /// \param settings A settings object described how to create a new CGE object.
    /// \return A new CGE object.
    static CGE* new_CGE(const Settings& settings = Settings());
};

} // namespace Muninn

#endif /* MUNINN_CGEFACTORY_H_ */

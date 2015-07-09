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
enum GeEnum {GE_MULTICANONICAL=0, GE_INV_K, GE_INV_K_P, GE_ENUM_SIZE};

/// Define the string names corresponding to the values of GeEnum.
static const std::string GeEnumNames[] = {"multicanonical", "invk", "invkp"};

/// Define the different estimators
enum EstimatorEnum {ESTIMATOR_MLE=0, ESTIMATOR_ENUM_SIZE};

/// Define the string names corresponding to the values of EstimatorEnum
static const std::string EstimatorEnumNames[] = {"MLE"};

/// Define the different binners.
enum BinnerEnum {DYNAMIC=0, UNIFORM, UNIFORM_MIN_MAX, BINNER_ENUM_SIZE};

/// Define the string names corresponding to the values of GeEnum.
static const std::string BinnerEnumNames[] = {"dynamic", "uniform", "uniform-min-max"};

/// Input operator of a GeEnum from string.
std::istream &operator>>(std::istream &input, GeEnum &g);

/// Output operator for a GeEnum.
std::ostream &operator<<(std::ostream &o, const GeEnum &g);

/// Input operator of a EstimatorEnum from string.
std::istream &operator>>(std::istream &input, EstimatorEnum &g);

/// Output operator for a EstimatorEnum.
std::ostream &operator<<(std::ostream &o, const EstimatorEnum &g);

/// Input operator of a BinnerEnum from string.
std::istream &operator>>(std::istream &input, BinnerEnum &g);

/// Output operator for a BinnerEnum.
std::ostream &operator<<(std::ostream &o, const BinnerEnum &g);

/// Input operator of a StatisticsLogger::Mode from string.
std::istream &operator>>(std::istream &input, StatisticsLogger::Mode &m);

/// Output operator for a StatisticsLogger::Mode
std::ostream &operator<<(std::ostream &o, const StatisticsLogger::Mode &m);

/// A factory for creating a CGE object based on a local settings object.
/// The factory function creates a new CGE class, and all additional classes will be
/// created automatically and transparently.
class CGEfactory {
public:
    /// Local settings class
    class Settings {
    public:
        /// Weight-scheme to use: invk|multicanonical|invkp
        GeEnum weight_scheme;

        /// Estimator to use: MLE
        EstimatorEnum estimator;

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

        /// The p value for the invkp scheme
        double p;

        /// The resolution for the dynamic binner. The binning is set so
        /// that a uniform resolution, \f$ r \f$, in the weights
        /// is obtained, \f$ |ln w(E_j) - \ln w(E_{j+1})| \simeq r \f$.
        double resolution;

        /// Use the initial bin with as maximal bin width, when expanding to the left.
        bool initial_width_is_max_left;

        /// Use the initial bin with as maximal bin width, when expanding to the right.
        bool initial_width_is_max_right;

        /// The filename for writing the Muninn statistics logfile.
        std::string statistics_log_filename;

        /// Muninn log mode (current|all). See Muninn::StatisticsLogger::Mode
        /// for details.
        Muninn::StatisticsLogger::Mode log_mode;

        /// The precision (number of significant digits) used when writing
        /// floating point values to the log file.
        int log_precision;

        /// The statistics log is first read from statistics_log_filename and
        /// the result of the simulation is appended to this file.
        bool continue_statistics_log;

        /// The filename for reading the Muninn statistics logfile. If the
        /// value difference from the empty string (""), this log file is read
        /// and the history is set based on the content.
        std::string read_statistics_log_filename;

        /// The filename for reading a set of fixed weights for a given region.
        /// If the value difference from the empty string (""), this file is
        /// read and the weights are fixed in the given region.
        std::string read_fixed_weights_filename;

        /// Number of iterations used in first round of sampling.
        unsigned int initial_max;

        /// The sampling time between consecutive histograms is multiplied with
        /// this factor if the size of the support is not increased for the new
        /// histogram.
        double increase_factor;

        /// The maximum sampling time (number of iteration) for each round of sampling.
        Count max_iterations_per_histogram;

        /// The number of consecutive histograms to keep in memory.
        unsigned int memory;

        /// The minimal number of counts in a bin in order to have support in that bin.
        unsigned int min_count;

        /// Restrict the support of the individual histograms to only cover the support for the given histogram.
        bool restricted_individual_support;

        /// Which binner to use
        BinnerEnum binner;

        /// The maximal number of bins allowed to be used by the binner
        unsigned int max_number_of_bins;

        /// Bin width used for the uniform binner (used when binner==UNIFORM)
        double bin_width;

        /// The initial min value for the uniform binner (used when binner==UNIFORM_MIN_MAX)
        double binner_min_value;

        /// The initial max value for the uniform binner (used when binner==UNIFORM_MIN_MAX)
        double binner_max_value;

        /// The initial number of bins for the uniform binner (used when binner==UNIFORM_MIN_MAX)
        unsigned int binner_nbins;

        /// The separator symbol between option name and value used in the output operator of the settings object.
        std::string separator;

        /// The verbose level of Muninn.
        int verbose;

        /// Constructor that sets the default values for the settings.
        ///
        /// \param weight_scheme See documentation for Settings::weight_scheme.
        /// \param estimator See documentation for Settings::estimator.
        /// \param slope_factor_up See documentation for Settings::slope_factor_up.
        /// \param slope_factor_down See documentation for Settings::slope_factor_down
        /// \param min_beta See documentation for Settings::min_beta.
        /// \param max_beta See documentation for Settings::max_beta.
        /// \param initial_beta See documentation for Settings::initial_beta.
        /// \param p See documentation for Settings::p.
        /// \param resolution See documentation for Settings::resolution.
        /// \param initial_width_is_max_left See documentation for Settings::initial_width_is_max_left.
        /// \param initial_width_is_max_right See documentation for Settings::initial_width_is_max_right.
        /// \param statistics_log_filename See documentation for Settings::statistics_log_filename.
        /// \param log_mode See documentation for Settings::log_mode.
        /// \param log_precision See documentation for log_precision.
        /// \param continue_statistics_log See documentation for continue_statistics_log.
        /// \param read_statistics_log_filename See documentation for Settings::read_statistics_log_filename.
        /// \param read_fixed_weights_filename See documentation for Settings::read_fixed_weights_filename.
        /// \param initial_max  See documentation for Settings::initial_max.
        /// \param increase_factor  See documentation for Settings::increase_factor.
        /// \param max_iterations_per_histogram See documentation for Settings::max_iterations_between_rounds.
        /// \param memory See documentation for Settings::memory.
        /// \param min_count See documentation for Settings::min_count.
        /// \param restricted_individual_support See documentation for Settings::restricted_individual_support.
        /// \param binner See documentation for Settings::binner
        /// \param max_number_of_bins See documentation for Settings::max_number_of_bins.
        /// \param bin_width See documentation for Settings::bin_width.
        /// \param binner_min_value See documentation for Settings::binner_min_value.
        /// \param binner_max_value See documentation for Settings::binner_max_value.
        /// \param binner_nbins See documentation for Settings::binner_nbins.
        /// \param separator See documentation for Settings::separator.
        /// \param verbose See documentation for Settings::verbose.
        Settings(GeEnum weight_scheme=GE_MULTICANONICAL,
                 EstimatorEnum estimator=ESTIMATOR_MLE,
                 double slope_factor_up = 0.3,
                 double slope_factor_down = 3.0,
                 double min_beta=-std::numeric_limits<double>::infinity(),
                 double max_beta=std::numeric_limits<double>::infinity(),
                 double initial_beta=0.0,
                 double p=0.5,
                 double resolution = 0.2,
                 bool initial_width_is_max_left=true,
                 bool initial_width_is_max_right=false,
                 std::string statistics_log_filename = "muninn.txt",
                 Muninn::StatisticsLogger::Mode log_mode = Muninn::StatisticsLogger::ALL,
                 int log_precision = 10,
                 bool continue_statistics_log = false,
                 std::string read_statistics_log_filename = "",
                 std::string read_fixed_weights_filename = "",
                 unsigned int initial_max = 5000,
                 double increase_factor = 1.07,
                 Count max_iterations_per_histogram = std::numeric_limits<Count>::max(),
                 unsigned int memory = 40,
                 unsigned int min_count = 30,
                 bool restricted_individual_support=false,
                 BinnerEnum binner=DYNAMIC,
                 unsigned int max_number_of_bins=1000000,
                 double bin_width = 0.1,
                 double binner_min_value=0,
                 double binner_max_value=1,
                 unsigned int binner_nbins=1,
                 std::string separator=":",
                 int verbose=3)
        : weight_scheme(weight_scheme),
          estimator(estimator),
          slope_factor_up(slope_factor_up),
          slope_factor_down(slope_factor_down),
          min_beta(min_beta),
          max_beta(max_beta),
          initial_beta(initial_beta),
          p(p),
          resolution(resolution),
          initial_width_is_max_left(initial_width_is_max_left),
          initial_width_is_max_right(initial_width_is_max_right),
          statistics_log_filename(statistics_log_filename),
          log_mode(log_mode),
          log_precision(log_precision),
          continue_statistics_log(continue_statistics_log),
          read_statistics_log_filename(read_statistics_log_filename),
          read_fixed_weights_filename(read_fixed_weights_filename),
          initial_max(initial_max),
          increase_factor(increase_factor),
          max_iterations_per_histogram(max_iterations_per_histogram),
          memory(memory),
          min_count(min_count),
          restricted_individual_support(restricted_individual_support),
          binner(binner),
          max_number_of_bins(max_number_of_bins),
          bin_width(bin_width),
          binner_min_value(binner_min_value),
          binner_max_value(binner_max_value),
          binner_nbins(binner_nbins),
          separator(separator),
          verbose(verbose) {}

        /// Function for setting the separator symbol.
        ///
        /// \param new_separator The new value for the separator.
        /// \return A pointer to the Settings object.
        Settings* set_separator(std::string new_separator) {
            separator = new_separator;
            return this;
        }

        /// Output operator
        friend std::ostream &operator<<(std::ostream &o, const Settings &settings) {
            o << "weight_scheme" << settings.separator << settings.weight_scheme << std::endl;
            o << "estimator" << settings.separator << settings.estimator << std::endl;
            o << "slope_factor_up" << settings.separator << settings.slope_factor_up << std::endl;
            o << "slope_factor_down" << settings.separator << settings.slope_factor_down << std::endl;
            o << "min_beta" << settings.separator << settings.min_beta << std::endl;
            o << "max_beta" << settings.separator << settings.max_beta << std::endl;
            o << "initial_beta" << settings.separator << settings.initial_beta << std::endl;
            o << "p" << settings.separator << settings.p << std::endl;
            o << "resolution" << settings.separator << settings.resolution << std::endl;
            o << "initial_width_is_max_left" << settings.separator << settings.initial_width_is_max_left << std::endl;
            o << "initial_width_is_max_right" << settings.separator << settings.initial_width_is_max_right << std::endl;
            o << "statistics_log_filename" << settings.separator << settings.statistics_log_filename << std::endl;
            o << "log_mode" << settings.separator << settings.log_mode << std::endl;
            o << "log_precision" << settings.separator << settings.log_precision << std::endl;
            o << "continue_statistics_log" << settings.continue_statistics_log << std::endl;
            o << "read_statistics_log_filename" << settings.separator << settings.read_statistics_log_filename << std::endl;
            o << "read_fixed_weights_filename" << settings.separator << settings.read_fixed_weights_filename << std::endl;
            o << "initial_max" << settings.separator << settings.initial_max << std::endl;
            o << "increase_factor" << settings.separator << settings.increase_factor << std::endl;
            o << "max_iterations_per_histogram" << settings.separator << settings.max_iterations_per_histogram << std::endl;
            o << "memory" << settings.separator << settings.memory << std::endl;
            o << "min_count" << settings.separator << settings.min_count << std::endl;
            o << "restricted_individual_support" << settings.separator << settings.restricted_individual_support << std::endl;
            o << "binner" << settings.separator << settings.binner << std::endl;
            o << "max_number_of_bins" << settings.separator << settings.max_number_of_bins << std::endl;
            o << "bin_width" << settings.separator << settings.bin_width << std::endl;
            o << "binner_min_value"  << settings.separator << settings.binner_min_value << std::endl;
            o << "binner_max_value" << settings.separator << settings.binner_max_value << std::endl;
            o << "binner_nbins" << settings.separator << settings.binner_nbins << std::endl;
            o << "verbose" << settings.separator << settings.verbose << std::endl;
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

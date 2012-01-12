// timer.h
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

#ifndef MUNINN_TIMER_H_
#define MUNINN_TIMER_H_

#include <ctime>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>

#include <sys/time.h>

#include "muninn/common.h"

namespace Muninn {

// A forward decelerations
class TimeCollector;
class Timer;

/// The global timer object
extern TimeCollector global_time_collector;

/// A class for accumulating Timer objects.
class TimeCollector {
public:

    /// Constructor for the class.
    ///
    /// \param name The name that is used, when the object is printed to stdout.
    TimeCollector(const std::string &name) : name(name) {}

    /// Default constructor. The accumulated times are printed upon destruction.
    ~TimeCollector() {
        if (time.size()>0)
            std::cout << (*this);
    }

    /// Add a Timer to the collector and accumulated the time corresponding to
    /// the key value.
    /// \param timer The timer to add.
    void add_time(const Timer &timer);

    /// Output operator for the TimeCollector class.
    friend std::ostream &operator<<(std::ostream &output, const TimeCollector &time_collector) {
        std::string underscores = "--------------------------";
        underscores.resize(underscores.length()+time_collector.name.length(), '-');
        std::cout << underscores << std::endl;
        std::cout << "TimeCollector summary for " << time_collector.name << std::endl << std::endl;

        // Find the max length of the names
        std::size_t max_len_key=0;
        unsigned int max_len_value=0;
        for (std::map<std::string, unsigned long int>::const_iterator it=time_collector.time.begin(); it!=time_collector.time.end(); ++it) {
            max_len_key = std::max(max_len_key, it->first.length());
            if (it->second > 0) {
                max_len_value = std::max(max_len_value, static_cast<unsigned int>(log10(it->second)));
            }
        }

        // Print out the values
        for (std::map<std::string, unsigned long int>::const_iterator it=time_collector.time.begin(); it!=time_collector.time.end(); ++it) {
            std::cout << std::setw(max_len_key) << std::left << it->first << ": ";
            std::cout << std::setw(max_len_value) << std::right << it->second << " us  (";
            std::cout << std::setw(10) << std::right << static_cast<double>(it->second)*1E-6 << " s)";
            std::cout << std::endl;
        }
        std::cout << underscores << std::endl;
        return output;
    }

private:
    std::string name;                               ///< Name of the TimeCollector.
    std::map<std::string, long unsigned int> time;  ///< Map between keys and accumulated time.
};

/// A simple class for timing the actuarial execution time of code. The timing
/// is started upon construction and finished when the object is destructed.
class Timer {
public:

    /// Construct a time and start timing.
    ///
    /// \param key_name The key used, when adding the timer to a TimeCollector.
    /// \param time_collector The collector to add the time to upon destruction.
    Timer(const std::string &key_name, TimeCollector &time_collector) :
        key_name(key_name), time_collector(time_collector) {
        start = get_time();
    }

    /// Default destructor. Upon destruction the timer is stopped and added
    /// to a TimeCollector.
    ~Timer() {
        end = get_time();
        time_collector.add_time(*this);
    }

    /// Get the current time.
    ///
    /// \return The current time
    static inline timeval get_time() {
         struct timeval tv;
         gettimeofday(&tv, NULL);
         return tv;
    }

    friend class TimeCollector;

protected:
    std::string key_name;           ///< The key used, when adding the timer to a TimeCollector.
    timeval start;                  ///< Timestamp set when the timer is constructed.
    timeval end;                    ///< Timestamp set when the timer is destructed.
    TimeCollector &time_collector;  ///< The TimeCollector to added the timer to upon destruction.
};

} /* namespace Muninn */

#endif /* MUNINN_TIMER_H_ */

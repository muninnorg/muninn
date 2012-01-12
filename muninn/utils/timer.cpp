// timer.cpp
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

#include "muninn/utils/timer.h"

namespace Muninn {

TimeCollector global_time_collector("Global");

void TimeCollector::add_time(const Timer &timer) {
    if (time.count(timer.key_name)==0)
        time[timer.key_name] = 0;
    long unsigned int diff = (timer.end.tv_sec-timer.start.tv_sec)*1000000 + (timer.end.tv_usec-timer.start.tv_usec);
    time[timer.key_name] += diff;
}

} // namespace Muninn

# CanonicalBase.py
# Copyright (c) 2010 Jes Frellsen
#
# This file is part of Muninn.
#
# Muninn is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# Muninn is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Muninn.  If not, see <http://www.gnu.org/licenses/>.
#
# The following additional terms apply to the Muninn software:
# Neither the names of its contributors nor the names of the
# organizations they are, or have been, associated with may be used
# to endorse or promote products derived from this software without
# specific prior written permission.

from parse_statics_log import parse_statics_log, convert_log_entry

from numpy import exp, log, sum, min
from utils import log_sum_exp


class CanonicalException(Exception):
    pass


class CanonicalBase:

    def __init__(self, statics_log_filename, which=-1):
        result = parse_statics_log(statics_log_filename, which)

        # Chech that the wanted information is in there
        if not (result.has_key('lnG') and len(result['lnG'])>=1):
            raise CanonicalException("lngG not found in statics log")

        if not (result.has_key('lnG_support') and len(result['lnG_support'])>=1):
            raise CanonicalException("lngG_support not found in statics log")

        if not (result.has_key('binning') and len(result['binning'])>=1):
            raise CanonicalException("binning not found in statics log")

        # Save the read variables
        (self.number, self.fullname, self.lnG) = convert_log_entry(result['lnG'][0])
        self.lnG_support = convert_log_entry(result['lnG_support'][0])[2]
        self.binning = convert_log_entry(result['binning'][0])[2]
        self.bin_widths = convert_log_entry(result['bin_widths'][0])[2]
        self.bin_centers = self.binning[:-1] + 0.5*(self.binning[1:]-self.binning[:-1])


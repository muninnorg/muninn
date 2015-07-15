# utils.py
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

from numpy import max, sum, exp, log
from cPickle import load, dump

def log_sum_exp(summands):
    m = max(summands)
    s = sum(exp(summands-m))
    return m + log(s)

def pickle_to_file(obj, file_name, protocol=2):
    """
    Pickle an object to a file
    """
    f = open(file_name, 'w')
    dump(obj, f, protocol)
    f.close()

def unpickle_from_file(file_name):
    """
    Unpickle a file
    """
    f = open(file_name, 'r')
    obj = load(f)
    f.close()
    return obj
                    

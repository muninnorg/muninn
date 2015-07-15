# parse_tarrays.py
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

import numpy as np
import re

re_float_pattern = "(?:[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)"
re_TArray_pattern = "TArray\(\s*\[(\s*(?:\[|\]|%s\s*)*)\]\s*,\s*type\s*=\s*(\w+)\s*,\s*shape=\s*\[(\s*(?:[0-9]+\s*))\]\s*\)" % re_float_pattern

data_types = {'i': np.int_, 'j': np.uint64, 'y': np.uint64, 'b': np.bool_, 'd': np.float_}


def text_to_array(text):
    """
    Convert a TArray from text representation to a numpy array
    """
    match = re.search(re_TArray_pattern, text)
    if match is not None:
        (data_string, type_string, shape_string) = match.groups()

        # Determine the type conversion
        dtype = data_types.get(type_string, np.float_)

        # Convert the data to numpy array and reshape it
        if type_string != 'b':
            data = np.array(data_string.split(), dtype=dtype)
        else:
            data = np.array(map(int, data_string.split()), dtype=dtype)
        shape = map(int, shape_string.split())
        data.reshape(shape, order='F')

        return data

    else:
        return None


if __name__ == "__main__":
    print text_to_array("TArray([18.1649 17.8739 14.7229 12.3586 11.2641 10.9244 9.63575], type=d, shape=[7])")
    print text_to_array("TArray([0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1], type=b, shape=[27])")

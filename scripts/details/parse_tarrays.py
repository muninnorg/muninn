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

from numpy import array
import re

re_float_pattern = "(?:[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)"
re_TArray_pattern =  "TArray\(\s*\[(\s*(?:\[|\]|%s\s*)*)\]\s*,\s*type\s*=\s*(\w+)\s*,\s*shape=\s*\[(\s*(?:[0-9]+\s*))\]\s*\)" % re_float_pattern

data_types = {'i':int, 'j':int, 'b':lambda val: bool(int(val)), 'd':float}

def text_to_array(text):
    """
    Convert a TArray from text representation to a numpy array
    """
    match = re.search(re_TArray_pattern, text)
    if match!=None:
        (data_string, type_string, shape_string) = match.groups()

        # Determine the type conversion
        dtype = data_types.get(type_string, float)

        # Convert the data and shape to values
        data = map(dtype, data_string.split())
        shape = map(int, shape_string.split())

        # Make the numpy array and reshape it
        a = array(data)
        a.reshape(shape, order='F')

        return a
        
    else:
        return None

    
def text_to_names_and_arrays(text):
    """
    Find all occurences of the pattern
    
      value[0-9]+ = TArray(...)
    
    In the text.
    """
    data = findall("\s*(\w+)\s*=\s*" + re_TArray_pattern, text)
    data = map(lambda (name, data, type, shape) :
               (name, map(float, findall(re_float_pattern, data)), type, map(float,findall(re_float_pattern, shape))), data)
    data = map(lambda (name, data, type, shape) : (name, array(data).reshape(tuple(shape), order='F')), data)
    return data


if __name__=="__main__":
    print text_to_array("TArray([18.1649 17.8739 14.7229 12.3586 11.2641 10.9244 9.63575], type=d, shape=[7])")
    print text_to_array("TArray([0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1], type=b, shape=[27])")

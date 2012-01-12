# parse_statics_log.py
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

import re
from parse_tarrays import text_to_array


def parse_statics_log(filename, start=None, end=None, indices=[]):
    """
    Parse the statics log and return a dictionary of the results.

    If start or end is different from None, only values in [start:end]
    will be read. Only start may be negative (meaning measured from
    the end).

    If a list of indices are given, only values in the list will be
    read.

    Returns a dictionary of list log entries. The format for a log
    entry is:

      entry = (number, fullname, tarray)

    Where number is a int, fullname str and tarray str. The list of
    log entries can be converted using the function convert_log_entries.
    """

    assert((start==None and end==None) or indices==[])

    result = {}

    fh = open(filename, 'r')

    # Parse the file line by line
    for line in fh:
        # Check if it a data line
        match = re.match("[ \n]*(([a-zA-Z_]+)([0-9]+))[ \n]*=[ \n]*(TArray\([^\n]+\))\n", line)

        if match!=None:
            (fullname, name, number, tarray) = match.groups()
            number = int(number)
            result.setdefault(name, [])

            if (start==None or start<0 or start<=number) and (end==None or number<end) and (indices==[] or (number in indices)):
                entry = (int(number), fullname, tarray)
                result[name].append(entry)

            if start!=None and start<0 and len(result[name])>-start:
                result[name].pop(0)
                         
    fh.close()
    return result


def convert_log_entry(entry):
    (number, fullname, tarray) = entry
    return (number, fullname, text_to_array(tarray))

def convert_log_entries(entries, default=None):
    if entries==None:
        return default
    else:
        return map(lambda (number, fullname, tarray) : (number, fullname, text_to_array(tarray)), entries)


if __name__=="__main__":
    parse_statics_log("../../bin/Muninn.txt", -1)


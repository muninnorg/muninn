# plot.py
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

import numpy
from numpy import array, log, zeros
from re import findall
from details.myhist import myhist, myhist2d
from details.utils import pickle_to_file

from rpy import r

def plot_name_array(name, data, ndims, counts=False, bins=None, support=None, makenewplot=True,
                    color="black", xlab=None, ylab="", xmin=None, xmax=None, main=None,
                    bin_numbers=True):

    # Set the title
    if main==None:
        this_main = name
    elif main=="":
        this_main = None
    else:
        this_main = main

    # Set the sub-title
    sub = None

    if counts:
        sub = "%d samples" % data.sum()

    # Check that the data is floats (othervise R might give problems)
    if data.dtype == numpy.int32:
       data = array(data, dtype=float)
    
    # Do the plotting
    if ndims==1:
        return myhist(data, color=color, main=this_main, bins=bins, support=support, makenewplot=makenewplot,
                      xlab=xlab, ylab=ylab, sub=sub, xmin=xmin, xmax=xmax, bin_numbers=bin_numbers)
    elif ndims==2:
        return myhist2d(data, main=this_main, theta=-120)
    else:
        print "Cannot plot %d dimensions!" % ndims

def plot_binning(entries, main=None):
    for entry in entries:
        number, fullname, tarray = entry

        if main==None:
            this_main = fullname
        else:
            this_main = main

        if len(tarray)>0:
            r.plot(range(len(tarray)), tarray, xlab="bin index", ylab="center value", main=this_main)

def plot_bin_widths(entries, log_space=False, main=None):
    for entry in entries:
        number, fullname, tarray = entry

        if log_space:
            tarray = numpy.log(tarray)
            ylab = r("expression(ln(Delta[bin]))")
        else:
            ylab = r("expression(Delta[bin])")

        if main==None:
            this_main = fullname
        else:
            this_main = main
        
        if len(tarray)>0:
            r.plot(range(len(tarray)), tarray, xlab="bin number", ylab=ylab, main=this_main)


def plot_entry_list(log_entry_list, binning=None, binning_dict=None, bin_widths=None, bin_widths_dict=None,
                    support_dict=None, xlab=None, ylab="", normalize_log_space=True, xmin=None, xmax=None,
                    main=None, bin_numbers=True):
    entries = convert_log_entries(log_entry_list)

    points = []

    for entry in entries:
        number, fullname, tarray = entry
        this_ylab = ylab

        # Decide on which binning to use
        if binning_dict!=None and binning_dict.has_key(number):
            bins = binning_dict[number][2]
        elif binning!=None and len(binning)==1 and len(binning[0][2])-1==len(tarray):
            bins = binning[0][2]
        else:
            bins = None

        # Normalize by bin width, if the binwidh is available 
        if bin_widths_dict!=None and bin_widths_dict.has_key(number):
            widths = bin_widths_dict[number][2]
        elif bin_widths!=None and len(bin_widths)==1 and len(bin_widths[0][2])==len(tarray):
            widths = bin_widths[0][2]
        else:
            widths = None

        if widths!=None:
            if normalize_log_space:
                tarray -= log(widths)
                this_ylab = r("expression(%s - ln(Delta[bin]))" % ylab)
            else:
                tarray = tarray/widths  # Note, /= worn't work, since tarray might be an int array
                this_ylab = r("expression(%s / Delta[bin])" % ylab)

        # Decide on which support to use
        if support_dict!=None and support_dict.has_key(number):
            support = support_dict[number][2]
        else:
            support = None

        # Do the plotting
        p = plot_name_array(fullname, tarray, 1, counts=False, bins=bins, support=support, makenewplot=True,
                            xlab=xlab, ylab=this_ylab, xmin=xmin, xmax=xmax,
                            main=main, bin_numbers=bin_numbers)

        points.append((number, fullname, p))

    return points

def plot_sum_N(log_entry_list, binning=None, binning_dict=None, bin_widths=None, bin_widths_dict=None,
                    support_dict=None, xlab=None, ylab="", normalize_log_space=True, xmin=None, xmax=None, main=None, bin_numbers=True):
    entries = convert_log_entries(log_entry_list)

    if binning!=None and binning_dict!=None and len(binning_dict)>0:
        newest = sorted(binning_dict.keys())[-1]

        # Added up the entries in the entry list
        bins = binning_dict[newest][2]
        sums = zeros(bins.shape[0]-1, dtype=entries[0][2].dtype)
        
        for entry in entries:
            number, fullname, tarray = entry

            # See if we need to align the binning
            if binning_dict.has_key(number):
                this_bins = binning_dict[number][2]

                size_diff = bins.shape[0] - this_bins.shape[0]

                # Check that this is not larger than the bins array
                if size_diff < 0:
                    print "Error aligning binning arrays: The newest array is smaller than a previous array."
                    return

                # Find the best alignment
                norm = lambda v: numpy.sqrt(numpy.dot(v,v))
                minimal = None


                for i in range(size_diff+1):
                    alignment_diff = norm(bins[i:bins.shape[0]-(size_diff-i)] - this_bins)

                    if minimal==None or alignment_diff < minimal[0]:
                        minimal = (alignment_diff, i)

                if minimal[0]>0.1:
                    print "Error aligning binning arrays: No good alignment found."
                    return
                    
                # Check that the count array has the right shape
                if (this_bins.shape[0]-1) != tarray.shape[0]:
                    print "Mismatch in size for binning and count array for iteration %d" % number
                    return

                # Add the counts to the sum
                sums[minimal[1]:((bins.shape[0]-1)-(size_diff-minimal[1]))] += tarray

            else:
                # No alignment needed
                # Check that the count array has the right shape
                if (bins.shape[0]-1) != tarray.shape[0]:
                    print "Mismatch in size for binning and count array for iteration %d" % number
                    return

                sums += tarray

        # Normalize by bin width, if the binwidh is available 
        if bin_widths_dict!=None and bin_widths_dict.has_key(newest):
            widths = bin_widths_dict[newest][2]
        else:
            widths = None

        if widths!=None:
            if normalize_log_space:
                sums -= log(widths)
                this_ylab = r("expression(%s - ln(Delta[bin]))" % ylab)
            else:
                sums = sums/widths  # Note, /= worn't work, since tarray might be an int array               
                this_ylab = r("expression(%s / Delta[bin])" % ylab)

            counts = False
            name = "sum_n"
        else:
            this_ylab = ylab
            counts = True
            name = "sum_N"


        # Do the plotting
        p = plot_name_array("Accumulated", sums, 1, counts=counts, bins=bins, support=None, makenewplot=True,
                            xlab=xlab, ylab=this_ylab, xmin=xmin, xmax=xmax, main=main, bin_numbers=bin_numbers)

        return [(None, name, p)]


if __name__ == "__main__":
    import os
    from details.parse_statics_log import parse_statics_log, convert_log_entries

    from optparse import OptionParser
    parser = OptionParser("usage: %prog [options]")
    parser.add_option("-f", dest="muninn_log_file", metavar="FILE", type="string", default=None, help="The Muninn statics-log filename [required].")
    parser.add_option("-o", dest="output", metavar="FILE", type="string", default="plot.pdf", help="The output plot file [default %default]")
    parser.add_option("-p", dest="pickle", metavar="FILE", type="string", default=None, help="Output the plot as a pickle [optional]")
    
    parser.add_option("-g", dest="lng", default=False, action="store_true", help="Plot the entropy (lng)")
    parser.add_option("-w", dest="lnw", default=False, action="store_true", help="Plot the weights (lnw)")
    parser.add_option("-n", dest="n", default=False, action="store_true", help="Plot the normalized couns")
    parser.add_option("-N", dest="N", default=False, action="store_true", help="Plot the counts (N)")
    parser.add_option("-s", dest="s", default=False, action="store_true", help="Plot the sum of normalized couns")
    parser.add_option("-S", dest="S", default=False, action="store_true", help="Plot the sum of counts (N)")
    parser.add_option("-b", dest="bins", default=False, action="store_true", help="Plot the binning")

    parser.add_option("--start", dest="start", metavar="VAL", default=None,
                      help="The first array number to plot, a negative value means " +
                           "counting from the end [default %default]")
    parser.add_option("--end", dest="end", metavar="VAL", default=None,
                      help="The last but one array number to plot, negative values " +
                           "are not allowed [default %default].")
    parser.add_option("-i", dest="indices", metavar="VAL", default=[], action="append",
                      help="Index for the array number to plot - options can be used multiple times.")
    parser.add_option("--xmin", dest="xmin", metavar="FLOAT", type="float", default=None,
                      help="The minimal x-value to plot.")
    parser.add_option("--xmax", dest="xmax", metavar="FLOAT", type="float", default=None,
                      help="The maximal x-value to plot.")
    parser.add_option("--width", dest="width", metavar="FLOAT", type="float", default=7.,
                      help="The width of the graphics region in inches [default %default].")
    parser.add_option("--height", dest="height", metavar="FLOAT", type="float", default=7.,
                      help="The height of the graphics region in inches [default %default].")
    parser.add_option("--cex", dest="cex", metavar="FLOAT", type="float", default=1,
                      help="Scaling factor for the font size [default %default].")
    parser.add_option("--main", dest="main", metavar="VAL", type="string", default=None,
                      help="Set a different title than the default.")
    parser.add_option("--xlab", dest="xlab", metavar="VAL", type="string", default=None,
                      help="Set a different label on the x-axis than 'E'.")
    parser.add_option("--no-bins", dest="bin_numbers", action="store_false", default=True,
                      help="Disable bin numbers on the x-axis.")

    (options, args) = parser.parse_args()

    # Check that rpy is present
    try:
        from rpy import r
    except ImportError:
        parser.error("rpy is not install.")

    # Check arguments
    if len(args)>0:
        parser.error("No additional arguments should be given.")

    # Check options
    if options.muninn_log_file==None:
        parser.error("Muninn statics-log filename must be given with the Option -f.")

    if not os.access(options.muninn_log_file, os.R_OK):
        parser.error("File '" + options.muninn_log_file + "' not accessible.")

    if (options.start!=None or options.end!=None) and options.indices!=[]:
        parser.error("Options --start and --end are mutually exclusive with -i.")

    if options.start!=None:
        try:
            options.start = int(options.start)
        except ValueError:
            parser.error("Invalid start value '" + options.start + "'.")

    if options.end!=None:
        try:
            options.end = int(options.end)
        except ValueError:
            parser.error("Invalid end value '" + options.end + "'.")

        if options.end<0:
            parser.error("Negative end value not allowed.")

    if options.indices!=[]:
        try:
            options.indices = map(int, options.indices)
        except ValueError:
            parser.error("Invalid index value used with option -i.")

            
    # Parse the log file
    log_dict = parse_statics_log(options.muninn_log_file, options.start, options.end, options.indices)

    # Convert the binning from strings to arrays and make a dictionary
    binning = convert_log_entries(log_dict.get('binning', []))
    binning_dict = dict(map(lambda entry: (entry[0], entry), binning))

    bin_widths = convert_log_entries(log_dict.get('bin_widths', []))
    bin_widths_dict = dict(map(lambda entry: (entry[0], entry), bin_widths))


    # Plot the required output
    r.pdf(options.output, width=options.width, height=options.height)
    r.par(cex=options.cex)

    xmin = options.xmin
    xmax = options.xmax

    points = []
        
    if options.lng:
        support = convert_log_entries(log_dict.get('lnG_support', []))
        support_dict = dict(map(lambda entry: (entry[0], entry), support))
        p = plot_entry_list(log_dict.get('lnG', []), binning, binning_dict, bin_widths, bin_widths_dict, support_dict,
                            xlab=options.xlab, ylab="ln(G)", xmin=xmin, xmax=xmax, main=options.main,
                            bin_numbers=options.bin_numbers)
        points += p

    if options.lnw:
        p = plot_entry_list(log_dict.get('lnw', []), binning, binning_dict, None, None,
                               xlab=options.xlab, ylab="ln(w)", xmin=xmin, xmax=xmax, main=options.main,
                               bin_numbers=options.bin_numbers)
        points += p

    if options.n:
        p = plot_entry_list(log_dict.get('N',[]), binning, binning_dict, bin_widths, bin_widths_dict,
                            xlab=options.xlab, ylab="N", normalize_log_space=False, xmin=xmin, xmax=xmax,
                            main=options.main, bin_numbers=options.bin_numbers)
        points += p

    if options.N:
        p = plot_entry_list(log_dict.get('N',[]), binning, binning_dict, None, None,
                            xlab=options.xlab, ylab="N", normalize_log_space=False, xmin=xmin, xmax=xmax,
                            main=options.main, bin_numbers=options.bin_numbers)
        points += p


    if options.s:
        p = plot_sum_N(log_dict.get('N',[]), binning, binning_dict, bin_widths, bin_widths_dict,
                       xlab=options.xlab, ylab="N", normalize_log_space=False, xmin=xmin, xmax=xmax,
                       main=options.main, bin_numbers=options.bin_numbers)
        points += p

    if options.S:
        p = plot_sum_N(log_dict.get('N',[]), binning, binning_dict, None, None,
                       xlab=options.xlab, ylab="N", normalize_log_space=False, xmin=xmin, xmax=xmax,
                       main=options.main, bin_numbers=options.bin_numbers)
        points += p
        
    if options.bins:
        plot_binning(binning, main=options.main)
        plot_bin_widths(bin_widths, log_space=False, main=options.main)
        plot_bin_widths(bin_widths, log_space=True, main=options.main)


    r.graphics_off()


    # Dump the output as a pickle
    if options.pickle!=None:
        pickle_to_file(points, options.pickle)
    

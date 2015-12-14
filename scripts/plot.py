# plot.py
# Copyright (c) 2010,2014 Jes Frellsen
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
from muninn.plot_histogram import plot_histogram
from muninn.utils import pickle_to_file


def plot_name_array(pdf, name, data, ndims, counts=False, bins=None, support=None, fig=None,
                    xlab=None, ylab="", xmin=None, xmax=None, main=None,
                    bin_numbers=True, color="blue"):

    # Set the title
    if main is None:
        this_main = name
    elif main == "":
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
    if ndims == 1:
        return plot_histogram(pdf, data, color=color, main=this_main, bins=bins, support=support, fig=fig,
                              xlab=xlab, ylab=ylab, sub=sub, xmin=xmin, xmax=xmax, bin_numbers=bin_numbers)
    else:
        print "Cannot plot %d dimensions!" % ndims


def plot_binning(pdf, entries, main=None, color="blue"):
    for entry in entries:
        number, fullname, tarray = entry

        if main is None:
            this_main = fullname
        else:
            this_main = main

        if len(tarray) > 0:
            fig = plt.figure()
            ax = fig.add_subplot(111)
            ax.plot(range(len(tarray)), tarray, color=color)
            ax.set_xlabel("bin index")
            ax.set_ylabel("center value")
            ax.set_title(this_main)
            pdf.savefig()


def plot_bin_widths(pdf, entries, log_space=False, main=None, color="blue"):
    for entry in entries:
        number, fullname, tarray = entry

        if log_space:
            tarray = numpy.log(tarray)
            ylab = r"$\ln(\Delta_\mathrm{bin})$"
        else:
            ylab = r"$\Delta_\mathrm{bin})$"

        if main is None:
            this_main = fullname
        else:
            this_main = main

        if len(tarray) > 0:
            fig = plt.figure()
            ax = fig.add_subplot(111)
            ax.plot(range(len(tarray)), tarray, color=color)
            ax.set_xlabel("bin number")
            ax.set_ylabel(ylab)
            ax.set_title(this_main)
            pdf.savefig()


def plot_entry_list(pdf, log_entry_list, binning=None, binning_dict=None, bin_widths=None, bin_widths_dict=None,
                    support_dict=None, xlab=None, ylab="", normalize_log_space=True, xmin=None, xmax=None,
                    main=None, bin_numbers=True, color="blue"):
    entries = convert_log_entries(log_entry_list)

    points = []

    for entry in entries:
        number, fullname, tarray = entry
        this_ylab = ylab

        # Decide on which binning to use
        if binning_dict is not None and number in binning_dict:
            bins = binning_dict[number][2]
        elif binning is not None and len(binning) == 1 and len(binning[0][2])-1 == len(tarray):
            bins = binning[0][2]
        else:
            bins = None

        # Normalize by bin width, if the binwidh is available
        if bin_widths_dict is not None and number in bin_widths_dict:
            widths = bin_widths_dict[number][2]
        elif bin_widths is not None and len(bin_widths) == 1 and len(bin_widths[0][2]) == len(tarray):
            widths = bin_widths[0][2]
        else:
            widths = None

        if widths is not None:
            if normalize_log_space:
                tarray -= log(widths)
                this_ylab = r"$%s - \ln(\Delta_\mathrm{bin})$" % ylab.replace("$", "")
            else:
                tarray = tarray/widths  # Note, /= worn't work, since tarray might be an int array
                this_ylab = r"$%s / \Delta_\mathrm{bin}$" % ylab.replace("$", "")

        # Decide on which support to use
        if support_dict is not None and number in support_dict:
            support = support_dict[number][2]
        else:
            support = None

        # Do the plotting
        p = plot_name_array(pdf, fullname, tarray, 1, counts=False, bins=bins, support=support, fig=None,
                            xlab=xlab, ylab=this_ylab, xmin=xmin, xmax=xmax,
                            main=main, bin_numbers=bin_numbers, color=color)

        points.append((number, fullname, p))

    return points


def l2norm(v):
    return numpy.sqrt(numpy.dot(v, v))


def plot_sum_N(pdf, log_entry_list, binning=None, binning_dict=None, bin_widths=None, bin_widths_dict=None,
               support_dict=None, xlab=None, ylab="", normalize_log_space=True, xmin=None, xmax=None, main=None, bin_numbers=True, color="blue"):
    entries = convert_log_entries(log_entry_list)

    if binning is not None and binning_dict is not None and len(binning_dict) > 0:
        newest = sorted(binning_dict.keys())[-1]

        # Added up the entries in the entry list
        bins = binning_dict[newest][2]
        sums = zeros(bins.shape[0]-1, dtype=entries[0][2].dtype)

        for entry in entries:
            number, fullname, tarray = entry

            # See if we need to align the binning
            if number in binning_dict:
                this_bins = binning_dict[number][2]

                size_diff = bins.shape[0] - this_bins.shape[0]

                # Check that this is not larger than the bins array
                if size_diff < 0:
                    print "Error aligning binning arrays: The newest array is smaller than a previous array."
                    return

                # Find the best alignment
                minimal = None

                for i in range(size_diff+1):
                    alignment_diff = l2norm(bins[i:bins.shape[0]-(size_diff-i)] - this_bins)

                    if minimal is None or alignment_diff < minimal[0]:
                        minimal = (alignment_diff, i)

                if minimal[0] > 0.1:
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
        if bin_widths_dict is not None and newest in bin_widths_dict:
            widths = bin_widths_dict[newest][2]
        else:
            widths = None

        if widths is not None:
            if normalize_log_space:
                sums -= log(widths)
                this_ylab = r"$%s - \ln(\Delta_\mathrm{bin})$" % ylab.replace("$", "")
            else:
                sums = sums/widths  # Note, /= worn't work, since tarray might be an int array
                this_ylab = r"$%s / \Delta_\mathrm{bin}$" % ylab.replace("$", "")

            counts = False
            name = "sum_n"
        else:
            this_ylab = ylab
            counts = True
            name = "sum_N"

        # Do the plotting
        p = plot_name_array(pdf, "Accumulated", sums, 1, counts=counts, bins=bins, support=None, fig=None,
                            xlab=xlab, ylab=this_ylab, xmin=xmin, xmax=xmax, main=main, bin_numbers=bin_numbers,
                            color=color)

        return [(None, name, p)]


if __name__ == "__main__":
    import os
    import matplotlib
    import matplotlib.pyplot as plt
    from matplotlib.backends.backend_pdf import PdfPages
    from muninn.parse_statics_log import parse_statics_log, convert_log_entries

    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", dest="muninn_log_file", metavar="FILE", type=str, required=True, help="The Muninn statics-log filename.")
    parser.add_argument("-o", dest="output", metavar="FILE", type=str, default="plot.pdf", help="The output plot file (default: %(default)s)")
    parser.add_argument("-p", dest="pickle", metavar="FILE", type=str, default=None, help="Output the plot as a pickle [optional]")
    
    parser.add_argument("-g", dest="lng", default=False, action="store_true", help="Plot the entropy (lng)")
    parser.add_argument("-w", dest="lnw", default=False, action="store_true", help="Plot the weights (lnw)")
    parser.add_argument("-n", dest="n", default=False, action="store_true", help="Plot the normalized counts")
    parser.add_argument("-N", dest="N", default=False, action="store_true", help="Plot the counts (N)")
    parser.add_argument("-s", dest="s", default=False, action="store_true", help="Plot the sum of normalized counts")
    parser.add_argument("-S", dest="S", default=False, action="store_true", help="Plot the sum of counts (N)")
    parser.add_argument("-b", dest="bins", default=False, action="store_true", help="Plot the binning")

    parser.add_argument("--start", dest="start", metavar="VAL", type=int, default=None,
                        help="The first array number to plot, a negative value means " +
                             "counting from the end (default: %(default)s)")
    parser.add_argument("--end", dest="end", metavar="VAL", type=int, default=None,
                        help="The last but one array number to plot, negative values " +
                             "are not allowed (default: %(default)s).")
    parser.add_argument("-i", dest="indices", metavar="VAL", default=[], action="append",
                        help="Index for the array number to plot - options can be used multiple times.")
    parser.add_argument("--xmin", dest="xmin", metavar="FLOAT", type=float, default=None,
                        help="The minimal x-value to plot.")
    parser.add_argument("--xmax", dest="xmax", metavar="FLOAT", type=float, default=None,
                        help="The maximal x-value to plot.")
    parser.add_argument("--width", dest="width", metavar="FLOAT", type=float, default=10.,
                        help="The width of the graphics region in inches (default: %(default)s).")
    parser.add_argument("--height", dest="height", metavar="FLOAT", type=float, default=7.,
                        help="The height of the graphics region in inches (default: %(default)s).")
    parser.add_argument("--fontsize", dest="fontsize", metavar="INT", type=int, default=14,
                        help="The font size (default: %(default)s).")
    parser.add_argument("--color", dest="color", metavar="COLOR", type=str, default="blue",
                        help="The plot color (default: %(default)s).")
    parser.add_argument("--main", dest="main", metavar="TEXT", type=str, default=None,
                        help="Set a different title than the default.")
    parser.add_argument("--xlab", dest="xlab", metavar="TEXT", type=str, default=None,
                        help="Set a different label on the x-axis than 'E'.")
    parser.add_argument("--no-bins", dest="bin_numbers", action="store_false", default=True,
                        help="Disable bin numbers on the x-axis.")
    args = parser.parse_args()

    # Check the arguments
    if not os.access(args.muninn_log_file, os.R_OK):
        parser.error("File '" + args.muninn_log_file + "' not accessible.")

    if (args.start is not None or args.end is not None) and args.indices != []:
        parser.error("Options --start and --end are mutually exclusive with -i.")

    if args.end is not None:
        if args.end < 0:
            parser.error("argument --end: negative end value not allowed.")

    if args.indices != []:
        try:
            args.indices = map(int, args.indices)
        except ValueError:
            parser.error("Invalid index value used with option -i.")

    # Parse the log file
    log_dict = parse_statics_log(args.muninn_log_file, args.start, args.end, args.indices)

    # Convert the binning from strings to arrays and make a dictionary
    binning = convert_log_entries(log_dict.get('binning', []))
    binning_dict = dict(map(lambda entry: (entry[0], entry), binning))

    bin_widths = convert_log_entries(log_dict.get('bin_widths', []))
    bin_widths_dict = dict(map(lambda entry: (entry[0], entry), bin_widths))

    # Plot the required output
    pdf = PdfPages(args.output)
    matplotlib.rc('font', size=args.fontsize)
    matplotlib.rc('figure', figsize=(args.width, args.height), max_open_warning=1000)

    xmin = args.xmin
    xmax = args.xmax

    points = []

    if args.lng:
        support = convert_log_entries(log_dict.get('lnG_support', []))
        support_dict = dict(map(lambda entry: (entry[0], entry), support))
        p = plot_entry_list(pdf, log_dict.get('lnG', []), binning, binning_dict, bin_widths, bin_widths_dict, support_dict,
                            xlab=args.xlab, ylab=r"$\ln(G)$", xmin=xmin, xmax=xmax, main=args.main,
                            bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.lnw:
        p = plot_entry_list(pdf, log_dict.get('lnw', []), binning, binning_dict, None, None,
                            xlab=args.xlab, ylab=r"$\ln(w)$", xmin=xmin, xmax=xmax, main=args.main,
                            bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.n:
        p = plot_entry_list(pdf, log_dict.get('N', []), binning, binning_dict, bin_widths, bin_widths_dict,
                            xlab=args.xlab, ylab=r"$N$", normalize_log_space=False, xmin=xmin, xmax=xmax,
                            main=args.main, bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.N:
        p = plot_entry_list(pdf, log_dict.get('N', []), binning, binning_dict, None, None,
                            xlab=args.xlab, ylab=r"$N$", normalize_log_space=False, xmin=xmin, xmax=xmax,
                            main=args.main, bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.s:
        p = plot_sum_N(pdf, log_dict.get('N', []), binning, binning_dict, bin_widths, bin_widths_dict,
                       xlab=args.xlab, ylab=r"$N$", normalize_log_space=False, xmin=xmin, xmax=xmax,
                       main=args.main, bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.S:
        p = plot_sum_N(pdf, log_dict.get('N', []), binning, binning_dict, None, None,
                       xlab=args.xlab, ylab=r"$N$", normalize_log_space=False, xmin=xmin, xmax=xmax,
                       main=args.main, bin_numbers=args.bin_numbers, color=args.color)
        points += p

    if args.bins:
        plot_binning(pdf, binning, main=args.main, color=args.color)
        plot_bin_widths(pdf, bin_widths, log_space=False, main=args.main, color=args.color)
        plot_bin_widths(pdf, bin_widths, log_space=True, main=args.main, color=args.color)

    pdf.close()

    # Dump the output as a pickle
    if args.pickle is not None:
        pickle_to_file(points, args.pickle)

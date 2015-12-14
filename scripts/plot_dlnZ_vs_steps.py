# plot_lnZ_vs_steps.py
# Copyright (c) 2015 Jes Frellsen
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
import bisect
import operator
import itertools
import muninn


def extract_muninn(filename, max_epochs, verbose):
    if verbose:
        print "Processing:", filename

    log = muninn.parse_statics_log(filename, end=max_epochs)

    N_list = zip(*muninn.convert_log_entries(log["N"]))[2]
    steps_list = np.cumsum(map(sum, N_list))
    lnG_list = zip(*muninn.convert_log_entries(log["lnG"]))[2]
    support_list = zip(*muninn.convert_log_entries(log["lnG_support"]))[2]
    binning_list = zip(*muninn.convert_log_entries(log["binning"]))[2]

    return {"steps_list": steps_list, "lnG_list": lnG_list, "support_list": support_list, "binning_list": binning_list, "fn": filename}


def calc_dlnZ(lnG, binning, support, beta0=0.0, beta1=1.0):
    cp = muninn.CanonicalPropertiesFromArrays(lnG, binning, support)

    lnZ0 = cp.lnZ(beta0)
    lnZ1 = cp.lnZ(beta1)

    return lnZ1 - lnZ0


def add_map_on_lng_extracts(extracts, function, key, **additional_f_kwargs):
    for extract in extracts:
        extract[key] = map(lambda (lnG, binning, support): function(lnG, binning, support, **additional_f_kwargs), zip(extract["lnG_list"], extract["binning_list"], extract["support_list"]))


def calc_running_mean_and_var_from_extracts(extracts, key, correct=None):
    X = sorted(list(set(itertools.chain.from_iterable(map(operator.itemgetter("steps_list"), extracts)))))

    Y = []
    Y_std = []

    for x in X:
        y = []
        for i, extract in enumerate(extracts):
            pos = bisect.bisect_right(extract["steps_list"], x) - 1
            if pos >= 0:
                y.append(extract[key][pos])
        y = np.array(y)
        Y.append(y.mean())
        Y_std.append(y.std())

    return np.array(X), np.array(Y), np.array(Y_std)


if __name__ == "__main__":
    import matplotlib
    matplotlib.use('PDF')
    import matplotlib.pyplot as plt
    from glob import glob

    # Setup commandline options
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("muninn_log_paths", metavar="FILE", nargs="+", type=str, help="the Muninn log files")
    parser.add_argument("-o", dest="plot_filename", metavar="FILE", type=str, default="dlnZ_vs_steps.pdf", help="the output plot file name (default: %(default)s)")
    parser.add_argument("-e", dest="max_epochs", metavar="INT", type=int, default=None, help="The last epoch to plot (default: %(default)s)")
    parser.add_argument("--beta0", dest="beta0", metavar="FLOAT", type=float, default=0., help="beta value for Z0 (beta0) (default: %(default)s)")
    parser.add_argument("--beta1", dest="beta1", metavar="FLOAT", type=float, default=1., help="beta value for Z1 (default: %(default)s)")
    parser.add_argument("--xlog", action="store_true", help="make the x-axis logarithmic")
    parser.add_argument("--color", metavar="COLOR", default="blue", type=str, help="line color for the plot (default: %(default)s)")
    parser.add_argument("--alpha", default=0.2, type=float, help="alpha levet for plotting std (default: %(default)s)")
    parser.add_argument("-t", dest="title", metavar="STR", type=str, default=None, help="Title of the plot (default: %(default)s)")
    parser.add_argument("--figsize", default=[20, 12], nargs=2, metavar="FLOAT", type=float, help="figure size in cm (default: %(default)s)")
    parser.add_argument("--subplotsAdj", default=None, nargs=4, metavar="FLOAT", type=float, help="arguments for subplot adjust (left=None, bottom=None, right=None, top=None)")
    parser.add_argument("--fontsize", metavar="INT", default=10, type=int, help="fontsize in pt (default: %(default)s)")
    parser.add_argument("--linewidth", metavar="FLOAT", default=0.2, type=float, help="line width for plotting (default: %(default)s)")
    parser.add_argument("--linewidthAxes", metavar="FLOAT", default=None, type=float, help="line width for the Axes (default: %(default)s)")
    parser.add_argument("-v", "--verbose", help="increase output verbosity", action="store_true")
    args = parser.parse_args()

    # Process the Muninn files
    files = itertools.chain(*map(glob, args.muninn_log_paths))

    extracts = map(lambda fn: extract_muninn(fn, args.max_epochs, args.verbose), files)
    add_map_on_lng_extracts(extracts, calc_dlnZ, "dlnZ_list", beta0=args.beta0, beta1=args.beta1)
    results = calc_running_mean_and_var_from_extracts(extracts, "dlnZ_list")

    # Plot the results
    matplotlib.rc('font', size=args.fontsize, family='serif')

    if args.linewidthAxes is not None:
        matplotlib.rcParams['axes.linewidth'] = args.linewidthAxes

    fig = plt.figure(figsize=map(lambda x: x/2.54, args.figsize))
    ax = fig.add_subplot(111)

    if args.subplotsAdj is not None:
        fig.subplots_adjust(*args.subplotsAdj)

    if args.title is not None:
        ax.set_title(args.title, fontweight='normal')

    if args.xlog:
        ax.set_xscale('log')

    ax.set_xlabel("MC steps")
    ax.set_ylabel(r"$\Delta \ln Z$")
    ax.ticklabel_format(style='sci', axis='x', scilimits=(0, 0))

    X, mean, std = results
    ax.plot(X, mean, color=args.color, linewidth=args.linewidth)
    ax.fill_between(X, mean-std, mean+std, edgecolor="none", facecolor=args.color, alpha=args.alpha)

    fig.savefig(args.plot_filename)

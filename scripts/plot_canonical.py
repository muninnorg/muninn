# plot_canonical.py
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
# specific prior written permission..

from details.CanonicalProperties import CanonicalProperties, CanonicalException

if __name__ == "__main__":
    import os
    from numpy import arange, vectorize
    import matplotlib
    import matplotlib.pyplot as plt
    from matplotlib.backends.backend_pdf import PdfPages
    from details.utils import pickle_to_file
    from details.parse_statics_log import parse_statics_log, convert_log_entries

    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", dest="muninn_log_file", metavar="FILE", type=str, required=True, help="The Muninn statics-log filename.")
    parser.add_argument("-o", dest="output", metavar="FILE", type=str, default="plot_canonical.pdf", help="The output plot file (default: %(default)s)")
    parser.add_argument("-p", dest="pickle", metavar="FILE", type=str, default=None, help="Output the plot as a pickle [optional]")
    parser.add_argument("-i", dest="which", metavar="VAL", type=int, default=-1, help="Which lng estimate to use in the statics-log (default: %(default)s)")
    parser.add_argument("--min", dest="inv_beta_min", metavar="VAL", default=0.1, type=float, help="Minimal value for 1/beta (default: %(default)s)")
    parser.add_argument("--max", dest="inv_beta_max", metavar="VAL", default=10,  type=float, help="Maximal value for 1/beta (default: %(default)s)")
    parser.add_argument("--width", dest="width", metavar="FLOAT", type=float, default=10.,
                      help="The width of the graphics region in inches (default: %(default)s).")
    parser.add_argument("--height", dest="height", metavar="FLOAT", type=float, default=7.,
                      help="The height of the graphics region in inches (default: %(default)s).")
    parser.add_argument("--fontsize", dest="fontsize", metavar="INT", type=int, default=14,
                      help="The font size (default: %(default)s).")
    parser.add_argument("--color", dest="color", metavar="COLOR", type=str, default="red",
                      help="The plot color (default: %(default)s).")
    args = parser.parse_args()

    # Check that the log file is accessible
    if not os.access(args.muninn_log_file, os.R_OK):
        parser.error("File '" + args.muninn_log_file + "' not accessible.")

    # Make the CanonicalProperties
    try:
        cp = CanonicalProperties(args.muninn_log_file, args.which)
    except CanonicalException, e:
        print parser.error(e)

    # Store all the plotting data
    data = []

    # Print which is used
    print "Using:", cp.fullname


    # Plot the required output
    pdf = PdfPages(args.output)
    matplotlib.rc('font', size=args.fontsize)
    matplotlib.rc('figure', figsize=(args.width, args.height), max_open_warning=1000)

    inv_beta = arange(args.inv_beta_min, args.inv_beta_max, 0.01)
    beta = 1.0/inv_beta

    lnZ = vectorize(cp.lnZ)(beta)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inv_beta, lnZ, color=args.color)
    ax.set_xlabel(r"$\beta^{-1}$")
    ax.set_ylabel(r"$\ln Z_\beta$")
    pdf.savefig()
    data.append((cp.number, "lnZ", (inv_beta, lnZ)))

    betaF = vectorize(cp.betaF)(beta)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inv_beta, betaF, color=args.color)
    ax.set_xlabel(r"$\beta^{-1}$")
    ax.set_ylabel(r"$F(\beta) \beta$")
    pdf.savefig()
    data.append((cp.number, "betaF", (inv_beta, betaF)))

    S = vectorize(cp.S)(beta)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inv_beta, S, color=args.color)
    ax.set_xlabel(r"$\beta^{-1}$")
    ax.set_ylabel(r"$S(\beta) / k_\mathrm{B}$")
    pdf.savefig()
    data.append((cp.number, "S", (inv_beta, S)))

    E = vectorize(cp.E)(beta)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inv_beta, E, color=args.color)
    ax.set_xlabel(r"$\beta^{-1}$")
    ax.set_ylabel(r"$\bar{E}(\beta)$")
    pdf.savefig()
    data.append((cp.number, "E", (inv_beta, E)))

    C = vectorize(cp.C)(beta)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inv_beta, C, color=args.color)
    ax.set_xlabel(r"$\beta^{-1}$")
    ax.set_ylabel(r"$C(\beta) / k_\mathrm{B}$")
    pdf.savefig()
    data.append((cp.number, "C", (inv_beta, C)))
    
    pdf.close()

    # Dump the output as a pickle
    if args.pickle!=None:
        pickle_to_file(data, args.pickle)

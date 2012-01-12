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
    from details.utils import pickle_to_file
    from details.parse_statics_log import parse_statics_log, convert_log_entries

    from optparse import OptionParser
    parser = OptionParser("usage: %prog [options]")
    parser.add_option("-f", dest="muninn_log_file", metavar="FILE", type="string", default=None, help="The Muninn statics-log filename [required].")
    parser.add_option("-o", dest="output", metavar="FILE", type="string", default="plot_canonical.pdf", help="The output plot file [default %default]")
    parser.add_option("-p", dest="pickle", metavar="FILE", type="string", default=None, help="Output the plot as a pickle [optional]")
    parser.add_option("-i", dest="which", metavar="VAL", type=int, default=-1, help="Which lng estimate to use in the statics-log [default %default]")
    parser.add_option("--min", dest="inv_beta_min", metavar="VAL", default=0.1, type="float", help="Minimal value for 1/beta [default %default]")
    parser.add_option("--max", dest="inv_beta_max", metavar="VAL", default=10,  type="float", help="Maximal value for 1/beta [default %default]")
    parser.add_option("--width", dest="width", metavar="FLOAT", type="float", default=7., help="The width of the graphics region in inches [default %default].")
    parser.add_option("--height", dest="height", metavar="FLOAT", type="float", default=7., help="The height of the graphics region in inches [default %default].")
    parser.add_option("--cex", dest="cex", metavar="FLOAT", type="float", default=1, help="Scaling factor for the font size [default %default].")

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

    # Make the CanonicalProperties
    try:
        cp = CanonicalProperties(options.muninn_log_file, options.which)
    except CanonicalException, e:
        print parser.error(e)

    # Store all the plotting data
    data = []

    # Print which is used
    print "Using:", cp.fullname


    # Plot the required output
    r.pdf(options.output, width=options.width, height=options.height)      
    r.par(cex=options.cex)

    inv_beta = arange(options.inv_beta_min, options.inv_beta_max, 0.01)
    beta = 1.0/inv_beta

    lnZ = vectorize(cp.lnZ)(beta)
    r.plot(inv_beta, lnZ, type='l', xlab=r("expression(beta**-1)"), ylab=r("""expression(paste("ln ", Z(beta)))"""))
    data.append((cp.number, "lnZ", (inv_beta, lnZ)))

    betaF = vectorize(cp.betaF)(beta)
    r.plot(inv_beta, betaF, type='l', xlab=r("expression(beta**-1)"), ylab=r("expression(F(beta) * beta)"))
    data.append((cp.number, "betaF", (inv_beta, betaF)))

    S = vectorize(cp.S)(beta)
    r.plot(inv_beta, S, type='l', xlab=r("expression(beta**-1)"), ylab=r("expression(S(beta) / k[B])"))
    data.append((cp.number, "S", (inv_beta, S)))

    E = vectorize(cp.E)(beta)
    r.plot(inv_beta, E, type='l', xlab=r("expression(beta**-1)"), ylab=r("expression(bar(E)(beta))"))
    data.append((cp.number, "E", (inv_beta, E)))

    C = vectorize(cp.C)(beta)
    r.plot(inv_beta, C, type='l', xlab=r("expression(beta**-1)"), ylab=r("expression(C(beta)/k[B])"))
    data.append((cp.number, "C", (inv_beta, C)))
    
    r.graphics_off()

    # Dump the output as a pickle
    if options.pickle!=None:
        pickle_to_file(data, options.pickle)

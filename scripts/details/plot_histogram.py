# plot_histogram.py
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
# specific prior written permission..

import operator
import matplotlib.pyplot as plt
from numpy import array, inf, arange

def plot_histogram(pdf, lnw, color="black", fig=None, main=None, bins=None, support=None, xlab=None, ylab="", sub=None, xmin=None, xmax=None, bin_numbers=True):
    nbins = len(lnw)

    if bins!=None:
        assert(nbins==len(bins)-1)

    xss = [[]]
    yss = [[]]
    connect_to_prev = False
    
    if bins==None:
        for bin in range(nbins):
            if (lnw[bin] > -inf or lnw[bin] < inf) and (support==None or support[bin]) and (xmin==None or xmin<=bin) and (xmax==None or (bin+1)<=xmax):
                xss[-1] += [bin, bin+1]
                yss[-1] += [lnw[bin], lnw[bin]]

            elif xss[-1]!=[]:
                xss.append([])
                yss.append([])
    else:
        for bin in range(nbins):
            if (lnw[bin] > -inf or lnw[bin] < inf) and (support==None or support[bin]) and (xmin==None or xmin<=bins[bin]) and (xmax==None or bins[bin+1]<=xmax):
                xss[-1] += [bins[bin], bins[bin+1]]
                yss[-1] += [lnw[bin], lnw[bin]]

            elif xss[-1]!=[]:
                xss.append([])
                yss.append([])

    # Do the plotting
    if fig==None:
        fig = plt.figure()
        ax = fig.add_subplot(111)

        ys = reduce(operator.add, yss, [])
        ylim = (min(ys), max(ys))
        
        if bins!=None and bin_numbers and False:
            # TODO: Add support for bin numbers
            this_xlab = r"$E$"
        elif bins!=None:
            this_xlab = r"$E$"
        else:
            this_xlab = r"bin"
        
        ax.set_ylabel(ylab)

        if xlab==None:
            ax.set_xlabel(this_xlab)
        else:
            ax.set_xlabel(xlab)
        
        if main!=None and sub!=None:
            ax.set_title(main + "\n" + sub)
        elif main!=None:
            ax.set_title(main)

    for (xs, ys) in zip(xss, yss):
        ax.plot(xs, ys, color=color, linewidth=0.5)

    pdf.savefig()

    return (xss, yss)

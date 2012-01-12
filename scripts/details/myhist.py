# myhist.py
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

import operator
from rpy import r
from numpy import array, inf, arange

def myhist(lnw, color="black", makenewplot=True, main=None, bins=None, support=None, xlab=None, ylab="", sub=None, xmin=None, xmax=None, bin_numbers=True):
    nbins = len(lnw)

    if bins!=None:
        assert(nbins==len(bins)-1)

    xss = [[]]
    yss = [[]]
    xlim = None
    connect_to_prev = False
    
    if bins==None:
        for bin in range(nbins):
            if (lnw[bin] > -inf or lnw[bin] < inf) and (support==None or support[bin]) and (xmin==None or xmin<=bin) and (xmax==None or bin<=xmax):
                xss[-1] += [bin, bin+1]
                yss[-1] += [lnw[bin], lnw[bin]]

            elif xss[-1]!=[]:
                xss.append([])
                yss.append([])

        if xmin==None and xmax==None:
            xlim = (0, nbins+1)
        else:
            xs = reduce(operator.add, xss, [])
            xlim = (min(xs), max(xs))
    else:
        for bin in range(nbins):
            if (lnw[bin] > -inf or lnw[bin] < inf) and (support==None or support[bin]) and (xmin==None or xmin<=bins[bin]) and (xmax==None or bins[bin]<=xmax):
                xss[-1] += [bins[bin], bins[bin+1]]
                yss[-1] += [lnw[bin], lnw[bin]]

            elif xss[-1]!=[]:
                xss.append([])
                yss.append([])

        if xmin==None and xmax==None:
            xlim = (bins[0], bins[-1])
        else:
            xs = reduce(operator.add, xss, [])
            xlim = (min(xs), max(xs))

    # Do the plotting
    if makenewplot:
        r.plot_new()

        ys = reduce(operator.add, yss, [])
        ymin = min(ys)
        ymax = max(ys)
        
        r.plot_window(xlim=xlim, ylim=(ymin, ymax))

        if bins!=None and bin_numbers:
            labels = array(r.pretty(range(nbins), 8), dtype=int)
            labels = labels[(0<=labels) & (labels<nbins)]
            ats = bins[labels]

            r.axis(1)
            r.axis(1, tick=False, labels=labels, at=ats, mgp=(3,2,0))
            r.axis(2)

            this_xlab = "E, bin"
        else:
            r.axis(1)
            r.axis(2)

            this_xlab = "E"
        
        r.axis(1)
        r.axis(2)
        r.box()
        r.title(ylab=ylab)

        if xlab==None:
            r.title(xlab=this_xlab)
        else:
            r.title(xlab=xlab)
        
        if main!=None:
            r.title(main=main)

        if sub!=None:
            r.title(sub=sub)

    for (xs, ys) in zip(xss, yss):
        r.lines(xs, ys, col=color, lwd=0.5)

    return (xss, yss)


def myhist2d(lnw, color="gray", main=None, theta=30):
    
    # Set up the array
    xs = arange(lnw.shape[0])
    ys = arange(lnw.shape[1])

    r.persp(xs, ys, lnw, theta = theta, phi = 30, expand = 0.5, col=color, xlab="i", ylab="j", zlab="S", main=main, ticktype="detailed")

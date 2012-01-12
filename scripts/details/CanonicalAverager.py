# CanonicalAverager.py
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

from CanonicalBase import CanonicalBase, CanonicalException
import numpy
from numpy import exp, log, sum, min, array
from utils import log_sum_exp

class CanonicalAverager(CanonicalBase):
    def __init__(self, statics_log_filename, which=-1):
        CanonicalBase.__init__(self, statics_log_filename, which)

    def calc_bin(self, energy):
        """
        Calculate the bin number for the given energy. A value of -1
        or len(self.binning), mean that the energy falls outside the
        bin boundaries.
        """
        return numpy.searchsorted(self.binning, energy, side='right') - 1       

    def calc_weights(self, energies, beta=1.0):
        """
        Calculate the canonical weights to be in a canonical average
        for a list of energies.
        """

        # Make a histogram of the energies
        histogram = numpy.histogram(energies, bins=self.binning)[0]

        # In some versions of numpy, there will be an extra bin, with
        # values faling outside the histogram
        if len(histogram)==len(self.lnG_support)+1:
            histogram = histogram[:-1]

        # Calculate the support between lnG and the histogram
        support = self.lnG_support & (histogram>0)

        if (numpy.any(support)):
            # Calculate the probability of each bin according to the
            # canonical ensemble within the common support
            lnGs = self.lnG[support]
            Es = self.bin_centers[support]

            lnZ = log_sum_exp(lnGs - beta*Es)

            P = numpy.zeros(support.shape)
            P[support] = exp(lnGs - beta*Es - lnZ)

            # Normalize the probabilities by the counts in the histogram
            P[support] /=  histogram[support]

            # Calculate the weight for each energy
            weights = []

            for energy in energies:
                bin_number = self.calc_bin(energy)

                if 0 <= bin_number < len(self.bin_centers):
                    weights.append(P[bin_number])
                else:
                    weights.append(0.0)

            return weights
        else:
            return [0.0 for energy in energies]



    def calc_average(self, energies, values, beta=1.0):
        """
        Calculate the canonical average using canonical weights at beta.
        """

        weights = self.calc_weights(energies, beta)
        average = numpy.average(values, weights=weights)
        return average


class CanonicalAveragerFixedEnergies(CanonicalBase):
    def __init__(self, statics_log_filename, energies, which=-1):
        CanonicalBase.__init__(self, statics_log_filename, which)

        # Save the energies
        self.energies = energies

        # Make a histogram of the energies
        self.histogram = numpy.histogram(energies, bins=self.binning)[0]

        # In some versions of numpy, there will be an extra bin, with
        # values faling outside the histogram
        if len(self.histogram)==len(self.lnG_support)+1:
            self.histogram = self.histogram[:-1]

        # Calculate the support between lnG and the histogram
        self.support = self.lnG_support & (self.histogram>0)

        if (numpy.any(self.support)):
            # Calculate the bin number for each energy
            self.bin_number_for_energies = []

            for energy in self.energies:
                bin_number = self.calc_bin(energy)

                if 0 <= bin_number < len(self.bin_centers):
                    self.bin_number_for_energies.append(bin_number)
                else:
                    self.bin_number_for_energies.append(-1)

    def calc_bin(self, energy):
        """
        Calculate the bin number for the given energy. A value of -1
        or len(self.binning), mean that the energy falls outside the
        bin boundaries.
        """
        return numpy.searchsorted(self.binning, energy, side='right') - 1       

    def calc_weights(self, beta=1.0):
        """
        Calculate the canonical weights to be in a canonical average
        for a list of energies.
        """

        if (numpy.any(self.support)):
            # Calculate the probability of each bin according to the
            # canonical ensemble within the common support
            lnGs = self.lnG[self.support]
            Es = self.bin_centers[self.support]

            lnZ = log_sum_exp(lnGs - beta*Es)

            P = numpy.zeros(self.support.shape)
            P[self.support] = exp(lnGs - beta*Es - lnZ)

            # Normalize the probabilities by the counts in the histogram
            P[self.support] /=  self.histogram[self.support]

            # Calculate the weight for each energy
            weights = []

            for bin_number in self.bin_number_for_energies:
                if 0 <= bin_number:
                    weights.append(P[bin_number])
                else:
                    weights.append(0.0)

            return weights
        else:
            return [0.0 for energy in self.energies]

    def calc_average(self, values, beta=1.0):
        """
        Calculate the canonical average using canonical weights at beta.
        """

        weights = self.calc_weights(beta)
        average = numpy.average(values, weights=weights)
        return average

if __name__=="__main__":
    ca = CanonicalAverager("../../bin/Muninn.txt")
    print ca.calc_weights([0.0, 0.0, 1.4])

# CanonicalProperties.py
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
from numpy import exp, log, sum, min
from utils import log_sum_exp


class CanonicalProperties(CanonicalBase):
    def __init__(self, statics_log_filename, which=-1):
        CanonicalBase.__init__(self, statics_log_filename, which)

        # Save lnG only where there is support
        self.lnGs = self.lnG[self.lnG_support]
        self.Es = self.bin_centers[self.lnG_support]

    def lnZ(self, beta):
        """
        Calculates the logarithm to partition function at beta
        """
        return log_sum_exp(self.lnGs - beta*self.Es)

    def Z(self, beta):
        """
        Calculates the partition function at beta
        """
        return exp(self.lnZ(beta))

    def PE(self, beta):
        """
        Calculates the probability of each energy bin at beta
        """
        return exp(self.lnGs - beta*self.Es - self.lnZ(beta))

    def ll(self, beta):
        """
        Calculates the log-likelihood of each energy bin at beta
        """
        return self.lnGs - beta*self.Es - self.lnZ(beta)
        
    def betaF(self, beta):
        """
        Calculates the free energy multiplied by beta
        """
        return -self.lnZ(beta)

    def E(self, beta):
        """
        Calculates the average energy at beta
        """

        return sum(self.Es * self.PE(beta))

    def Esq(self, beta):
        """
        Calculates the average square energy at beta
        """

        return sum(self.Es*self.Es * self.PE(beta))

    def S(self, beta):
        """
        Calculates the entropy at beta normalized by the bolztmann constant
        """
        
        return beta*self.E(beta) - self.betaF(beta)

    def C(self, beta):
        """
        Calculates the heat capacity normalized by the bolztmann constant at beta
        """

        E = self.E(beta)
        Esq = self.Esq(beta)

        return beta*beta*(Esq-E*E)


class CanonicalPropertiesFromArrays(CanonicalProperties):
    def __init__(self, lnG, binning, lnG_support=None):
        # Calculate the bin_centers
        bin_centers = binning[:-1] + 0.5*(binning[1:]-binning[:-1])

        # Save lnG only where there is support
        self.lnGs = lnG[lnG_support]
        self.Es = bin_centers[lnG_support]


if __name__=="__main__":
    ca = CanonicalProperties("../../bin/Muninn.txt")
    beta = 1E-4


    print "lnZ = ", ca.lnZ(beta)
    print "betaF = ", ca.betaF(beta)
    print "E = ", ca.E(beta)
    print "E^2 = ", ca.Esq(beta)
    print "S = ", ca.S(beta)
    print "C = ", ca.C(beta)
        

        

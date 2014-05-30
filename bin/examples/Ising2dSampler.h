// Ising2dSampler.h
// Copyright (c) 2010-2012 Jes Frellsen
//
// This file is part of Muninn.
//
// Muninn is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// Muninn is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Muninn.  If not, see <http://www.gnu.org/licenses/>.
//
// The following additional terms apply to the Muninn software:
// Neither the names of its contributors nor the names of the
// organizations they are, or have been, associated with may be used
// to endorse or promote products derived from this software without
// specific prior written permission.

#ifndef ISING2DSAMPLER_H_
#define ISING2DSAMPLER_H_

#include "details/random_utils.h"

#include "muninn/utils/TArray.h"

/// Class for generating correlated samples from a two-dimensional square-lattice
/// Ising model with a constant field and periodic boundary conditions.
///
/// Following MacKay  (2003, Cambridge University Press), the energy of the state
///  \f$\mathbf{x}\f$ is given by
///
/// \f[
///     E(\mathbf{x} | J, H) = - \left[ \frac{1}{2} \sum_{i,j} J_{i,j} x_i x_j + \sum_i H x_i \right]
/// \f]
///
/// where \f$J_{i,j}\f$ is the coupling for spins \f$x_i\f$ and \f$x_j\f$ and H is
/// the constant field. If \f$x_i\f$ and \f$x_j\f$ are neighbors then \f$J_{i,j}=J\f$,
/// otherwise \f$J_{i,j}=0\f$.
template <class ENERGY_TYPE>
class Ising2dSampler {
public:

	/// Constructor
	///
	/// \param N The size of the lattice sides.
	/// \param J The coupling parameter.
	/// \param H The external field.
	Ising2dSampler(unsigned int N, ENERGY_TYPE J=1, ENERGY_TYPE H=0) :
		N(N), J(J), H(H), X(N,N), E(0), term1(0), term2(0), can_undo(false), flipped_i(0), flipped_j(0) {
		reinitialize();
	}

	/// Reinitialize the Ising system
	void reinitialize() {
		// Reinitialize to a random starting state and calculate the energy
		for (Muninn::IArray::flatiterator it = X.get_flatiterator(); it(); ++it) {
			X(it) =  -1 + randomI(2)*2;
		}
		calculate_full_energy();
		can_undo = false;
	}

	/// Do a full energy calculation of the system.
	void calculate_full_energy() {
		term1 = 0;
		for (unsigned int i=0; i<N; ++i) {
			for (unsigned int j=0; j<N; ++j) {
				term1 += X(i,j) * X((i+1)%N,j);
				term1 += X(i,j) * X(i,(j+1)%N);
			}
		}

		term2 = X.sum();

		E = -(J*term1 + H*term2);
	}

	/// Make a move of the system, but flipping a random state updating the energy.
	void move() {
		// Flip a random position
		can_undo = true;
		flipped_i = randomI(N);
		flipped_j = randomI(N);

		move(flipped_i, flipped_j);
	}

	/// Undo the last move
	void undo() {
		// Reject can only be called once
		assert(can_undo);
		can_undo = false;

		// Reverse the move of the filled i and j
		move(flipped_i, flipped_j);
	}

	/// Get the energy associated with the current state of the system
	const ENERGY_TYPE& get_E() const {
		return E;
	}

	/// Get the current state of the system
	const Muninn::IArray& get_X() const {
		return X;
	}

	/// Check if the state if the system is consistent with the energy of
	/// the system.
	///
	/// \return True if the system is consistent and otherwise false.
	bool check_consistency() {
		ENERGY_TYPE old_E = E;
		calculate_full_energy();
		return E == old_E;
	}

private:

	unsigned int N;   /// The side size of the system.
	ENERGY_TYPE J;    /// The coupling parameter.
	ENERGY_TYPE H;    /// The field parameter.
	Muninn::IArray X; /// The state of the system.

	ENERGY_TYPE E;    /// The energy of the system.
	int term1;        /// The value of the first sum term in the energy
	int term2;        /// The value of the second sum term in the energy

	bool can_undo;          /// Is there a move to undo?
	unsigned int flipped_i; /// The first coordinate of the last flipped state
	unsigned int flipped_j; /// The second coordinate of the last flipped state

	/// Flip a given state in the system and update the energy.
	///
	/// \param i The first coordinate of the state to flip.
	/// \param j The second coordinate of the state to flip.
	void move(unsigned int i, unsigned int j) {
		// Mutate the state at (i,j)
		X(i, j) *= -1;

		// Calculate the change of energy in term1
		term1 += 2 * ( X(i, j) * X((i+1)%N, j) +
				       X(i, j) * X((N+i-1)%N, j) +
				       X(i, j) * X(i, (j+1)%N) +
				       X(i, j) * X(i, (N+j-1)%N) );

		// Calculate the change of energy in term2
		term2 += 2 * X(i, j);

		E = -(J*term1 + H*term2);
	}
};

#endif /* ISING2DSAMPLER_H_ */

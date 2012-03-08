// ArrayAligner.h
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

#ifndef MUNINN_ARRAYALIGNER_H_
#define MUNINN_ARRAYALIGNER_H_

#include <utility>

namespace Muninn {

/// Class with functions for aligning arrays.
class ArrayAligner {
public:

    /// Find the best alignment of an query array to a target array using only
	/// end gaps. The score of the alignment is the sum of square difference of
	/// the two aligned regions.
	///
	/// They arrays are assumed to be one dimensionally!
    ///
    /// \param target The array to align to.
    /// \param query The array to be aligned to the target. This array must be
	///              smaller than the target.
    /// \return A pair where the first entry is the offset on the left side of
	///         of the query (number of left end gaps) and the second is the
	///         alignment score.
	/// \tparam T The contents type of the vector.
	template<typename T>
	static std::pair<unsigned int, T> calculate_alignment_offset_and_score(const TArray<T> &target, const TArray<T> &query) {
		assert(target.get_ndims()==1 && query.get_ndims()==1);
		assert(target.get_shape(0) >= query.get_shape(0));

		// Find the optimal alignment of the two arrays
		T min_square_difference = std::numeric_limits<T>::max();
		unsigned int best_offset = 0;

		for (unsigned int offset=0; offset <= target.get_shape(0)-query.get_shape(0); ++offset) {
			T square_difference = 0;

			for (unsigned int index=0; index < query.get_shape(0); ++index) {
				square_difference += sq<T>(target(index+offset)-query(index));
			}

			if (square_difference < min_square_difference) {
				min_square_difference = square_difference;
				best_offset = offset;
			}
		}

		return std::pair<unsigned int, T>(best_offset, min_square_difference);
	}

    /// Find the best alignment of an query array to a target array using the
	/// function ArrayAligner::calculate_alignment_offset_and_score and return
	/// both the left and right offsets (number of end gaps).
	///
	/// They arrays are assumed to be one dimensionally!
    ///
    /// \param target The array to align to.
    /// \param query The array to be aligned to the target. This array must be
	///              smaller than the target.
    /// \return A pair where the first entry is the offset on the left side of
	///         of the query (number of left end gaps) and the second is the
	///         offset on the right side (number of right end gaps).
	/// \tparam T The contents type of the vector.
	template<typename T>
	static std::pair<unsigned int, unsigned int> calculate_alignment_offsets(const TArray<T> &target, const TArray<T> &query) {
		unsigned int offset_left = calculate_alignment_offset_and_score(target, query).first;
		unsigned int offset_right = target.get_shape(0) - query.get_shape(0) - offset_left;
		return std::pair<unsigned int, unsigned int>(offset_left, offset_right);
	}
};

} //namespace Muninn {

#endif // MUNINN_ARRAYALIGNER_H_

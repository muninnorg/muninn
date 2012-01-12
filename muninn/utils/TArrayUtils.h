// TArrayUtils.h
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

#ifndef MUNINN_TARRAYUTILS_H_
#define MUNINN_TARRAYUTILS_H_

#include <math.h>
#include <limits>

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

namespace Muninn {

/// Calculate the log-sum-exp of an array. That is, if you want to
/// calculate \f$ \ln \sum_i \exp(x_i) \f$, you can rewrite the expression as
/// \f[
///     \ln \sum_i \exp(x_i) = x_\textrm{max} + \ln \sum_i \exp(x_i-x_\textrm{max}),
/// \f]
/// where \f$ x_\textrm{max} = \max_i(x_i) \f$. We will call \f$ \{x_i\} \f$
/// for the summands.
///
/// \param summands The summands to calculate the log-sum-exp of.
/// \return The log-sum-exp value.
inline double log_sum_exp(const DArray &summands) {
    // Determine max of values
    double max = summands.max();

    // Do the summing
    double sum = 0;

    for (DArray::constflatiterator it = summands.get_constflatiterator(); it(); ++it)
        if (*it > -std::numeric_limits<double>::infinity())
            sum += exp(*it - max);

    return max + log(sum);
}

/// This function works as log_sum_exp, however the sum only runs over
/// a limited set of indices.
///
/// \param summands The summands to calculate the log-sum-exp of.
/// \param where The sum only runs over the indices, where this array is true.
/// \return The log-sum-exp value.
inline double log_sum_exp(const DArray &summands, const BArray &where) {
    // Determine max of values
    double max = summands.max(where);

    // Do the summing
    double sum = 0;

    for (BArray::constwheretrueiterator it = where.get_constwheretrueiterator(); it(); ++it)
        if (summands(it) > -std::numeric_limits<double>::infinity())
            sum += exp(summands(it) - max);

    return max + log(sum);
}

/// Find the index of the maximal element in an array.
///
/// \param array The array to find the maximal value in.
/// \return The index of the maximal value in the array.
template<typename T>
inline std::vector<unsigned int> arg_max(const TArray<T> &array) {
    T MAX = -std::numeric_limits<T>::infinity();
    int MAX_INDEX = 0;

    for (typename TArray<T>::constflatiterator it = array.get_constflatiterator(); it(); ++it) {
        if (*it > MAX) {
            MAX = *it;
            MAX_INDEX = it.get_index();
        }
    }
    return array.get_coord(MAX_INDEX);
}

/// Find the index of the maximal element in an array, but only among
/// a limited set of indices.
///
/// \param array The array to find the maximal value in.
/// \param where Only indices where this array is true is considered.
/// \return The index of the maximal value in the array.
template<typename T>
inline std::vector<unsigned int> arg_max(const TArray<T> &array, const BArray &where) {
    T MAX = -std::numeric_limits<T>::infinity();
    int MAX_INDEX = 0;

    for (BArray::constwheretrueiterator it = where.get_constwheretrueiterator(); it(); ++it) {
        if (array(it) > MAX) {
            MAX = array(it);
            MAX_INDEX = it.get_index();
        }
    }
    return array.get_coord(MAX_INDEX);
}

/// Find the number of elements that are true in an array.
///
/// \param array The array to find the number of true values in.
/// \return The number of true elements in the array.
inline unsigned int number_of_true(const BArray &array) {
    unsigned int count = 0;
    for(BArray::constflatiterator it = array.get_constflatiterator(); it(); ++it)
        if(*it)
            count++;
    return count;
}

/// Find the number of elements that are false in an array.
///
/// \param array The array to find the number of false values in.
/// \return The number of false elements in the array.
inline unsigned int number_of_false(const BArray &array) {
    unsigned int count = 0;
    for(BArray::constflatiterator it = array.get_constflatiterator(); it(); ++it)
        if(!*it)
            count++;
    return count;
}

/// Convert (by copying) a vector to a 1-dimensional array.
///
/// \param vector The vector to convert to an array.
/// \return A array representation of the vector.
///
/// \tparam T The type of the contents of the vector and the corresponding array.
template<typename T>
inline TArray<T> vector_to_TArray(const std::vector<T>& vector) {
    TArray<T> array(vector.size());
    for (unsigned int i=0; i<vector.size(); i++)
        array(i) = vector[i];
    return array;
}

/// Convert (by copying) an array to a vector.
///
/// \param array The array to convert to an vector.
/// \return A vector representation of the array.
///
/// \tparam T The type of the contents of the vector and the corresponding array.
template<typename T>
inline std::vector<T> TArray_to_vector(const TArray<T>& array) {
    std::vector<T> new_vector;
    for (typename TArray<T>::constflatiterator it=array.get_constflatiterator(); it(); ++it)
        new_vector.push_back(*it);
    return new_vector;
}

/// Copy values from one array to anther array on a limited set of indices. The
/// implementation uses the underlying arrays directly.
///
/// It is assumed that the from and where arrays has the same size, and that the
/// to array has at least the same size as the from array.
///
/// Note that the size of the arrays are note checked!
///
/// \param from The array to copy values from.
/// \param to The array to copy value to.
/// \param where Only copy the indices where this array is true.
void copy_to_flat(DArray &from, DArray &to, BArray &where);

/// Copy values from one array to anther array on a limited set of indices. The
/// implementation uses the underlying arrays directly.
///
/// It is assumed that the to and where arrays has the same size, and that the
/// from array has at least the same size as the from array.
///
/// Note that the size of the arrays are note checked!
///
/// \param from The array to copy values from.
/// \param to The array to copy value to.
/// \param where Only copy the indices where this array is true.
void copy_from_flat(DArray &from, DArray &to, BArray &where);

/// Works as copy_to_flat, by all values are negated when they are copied.
///
/// \param from The array to copy values from.
/// \param to The array to copy value to.
/// \param where Only copy the indices where this array is true.
void copy_to_flat_negate(DArray &from, DArray &to, BArray &where);

/// Works as copy_from_flat, by all values are negated when they are copied.
///
/// \param from The array to copy values from.
/// \param to The array to copy value to.
/// \param where Only copy the indices where this array is true.
void copy_from_flat_negate(DArray &from, DArray &to, BArray &where);

} // namespace Muninn


#endif /* MUNINN_TARRAYUTILS_H_ */

// TArrayMath.h
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

#ifndef MUNINN_TARRAYMATH_H_
#define MUNINN_TARRAYMATH_H_

#include "muninn/common.h"
#include "muninn/utils/TArray.h"

#include <cmath>

namespace Muninn {

/// Evaluated the trigonometric function cosine element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_cos(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::cos(array(it));

    return new_array;
}

/// Evaluated the trigonometric function sine element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_sin(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::sin(array(it));

    return new_array;
}

/// Evaluated the trigonometric function tangent element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_tan(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::tan(array(it));

    return new_array;
}

/// Evaluated the exponential function with base e element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_exp(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::exp(array(it));

    return new_array;
}

/// Evaluated the logarithm with base e element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_log(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::log(array(it));

    return new_array;
}

/// Evaluated the logarithm with base 10 element-wise on an array.
///
/// \param array The array to evaluated the function element-wise on.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_log10(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::log10(array(it));

    return new_array;
}

/// Evaluated the power function with a given double exponent and linear
/// multiplier 1 element-wise on an array.
///
/// \param base The array to evaluated the function element-wise on.
/// \param exponent The exponen of the power function.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_pow(const TARRAY &base, double exponent) {
    UARRAY new_array(base.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::pow(base(it), exponent);

    return new_array;
}

/// Evaluated the power function with a given integer exponent and linear
/// multiplier 1 element-wise on an array.
///
/// \param base The array to evaluated the function element-wise on.
/// \param exponent The exponent of the power function.
/// \return The result of evaluating the function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_pow(const TARRAY &base, int exponent) {
    UARRAY new_array(base.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::pow(base(it), exponent);

    return new_array;
}

/// Evaluated the square root element-wise on an array.
///
/// \param array The array to evaluated the root function element-wise on.
/// \return The result of evaluating the root function element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_sqrt(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::sqrt(array(it));

    return new_array;
}

/// Find absolute value element-wise on an array.
///
/// \param array The array to find the absolute value element-wise on.
/// \return The result of find the absolute value element wise on the array.
///
/// \tparam UARRAY The type of the return array.
/// \tparam TARRAY The type of the argument.
template<typename UARRAY, typename TARRAY>
inline UARRAY TArray_abs(const TARRAY &array) {
    UARRAY new_array(array.get_shape());

    for(typename UARRAY::flatiterator it = new_array.get_flatiterator(); it(); ++it)
        *it = std::abs(array(it));

    return new_array;
}

} // namespace Muninn

#endif /* MUNINN_TARRAYMATH_H_ */

// utils.h
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

#ifndef MUNINN_UTILS_H_
#define MUNINN_UTILS_H_

#include <cmath>
#include <vector>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <exception>

#include "muninn/common.h"

namespace Muninn {

/// Overload the output stream operator for std::deque.
template<typename T>
std::ostream& operator<<(std::ostream& output, const std::deque<T> & v) {
    output << "< ";
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(output, " "));
    output << ">";
    return output;
}

/// Overload the input stream operator for std::deque.
template<typename T>
std::ostream& operator<<(std::ostream& output, const std::vector<T> & v) {
    output << "< ";
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(output, " "));
    output << ">";
    return output;
}

/// Add the contents of two vectors element-wise. The function asserts that
/// the two vectors has the same size (using the assert function).
///
/// \param a The first summand.
/// \param b The second summand.
/// \return The element-wise sum of a and b.
///
/// \tparam The contents type of the vectors.
template<typename T>
std::vector<T> add_vectors(const std::vector<T> &a, const std::vector<T> &b) {
    assert(a.size()==b.size());
    std::vector<T> result(a);
    for(typename std::vector<T>::size_type i=0; i<a.size(); i++) {
        result[i] += b[i];
    }
    return result;
}

/// Add the contents of three vectors element-wise. The function asserts that
/// the three vectors has the same size (using the assert function).
///
/// \param a The first summand.
/// \param b The second summand.
/// \param c The third summand.
/// \return The element-wise sum of a, b and c.
///
/// \tparam The contents type of the vectors.
template<typename T>
std::vector<T> add_vectors(const std::vector<T> &a, const std::vector<T> &b, const std::vector<T> &c) {
    assert(a.size()==b.size());
    assert(a.size()==c.size());
    std::vector<T> result(a);
    for(typename std::vector<T>::size_type i=0; i<a.size(); i++) {
        result[i] += b[i] + c[i];
    }
    return result;
}

/// Evaluates if two vectors has the same size and all values of two vectors
/// are equal.
///
/// \param a The first vector to compare.
/// \param b The second vector to compare.
/// \return The true if the vectors has the same size and all elemnts are equal, otherwise false.
///
/// \tparam The contents type of the vectors.
template<typename T>
inline bool vector_equal(std::vector<T> a, std::vector<T> b) {
    // Check for same length
    if (a.size() != b.size())
        return false;

    // Check for same values
    for(typename std::vector<T>::size_type i=0; i<a.size(); i++)
        if (a[i]!=b[i])
            return false;

    // If the function hasn't returned the vectors must be equal.
    return true;
}

/// Calculate specified fractiles of a sequence of numbers. As a side effect
/// the the sequence of values are sorted.
///
/// \param values The sequence of values to find fractiles of.
/// \param fractions A sequence of fractional values to find fractiles at.
/// \return The fractiles corresponding to the given fractions.
std::vector<double> calculate_fractiles(std::vector<double> &values, const std::vector<double> &fractions);

/// Calculate the square of a number.
///
/// \param x The number to be squared.
/// \return The square of x (\f$ x^2 \f$).
template<typename T>
inline T sq(T x) {
  return x*x;
}

/// Create a vector with one element.
///
/// \param x0 The first element of the vector.
/// \return A vector containing the elements given as arguments.
///
/// \tparam T The contents type of the vector.
template<typename T>
inline std::vector<T> newvector(T x0) {
    std::vector<T> v(1);
    v[0] = x0;
    return v;
}

/// Create a vector with two element.
///
/// \param x0 The first element of the vector.
/// \param x1 The second element of the vector.
/// \return A vector containing the elements given as arguments.
///
/// \tparam T The contents type of the vector.
template<typename T>
inline std::vector<T> newvector(T x0, T x1) {
    std::vector<T> v(2);
    v[0] = x0;
    v[1] = x1;
    return v;
}

/// Create a vector with three element.
///
/// \param x0 The first element of the vector.
/// \param x1 The second element of the vector.
/// \param x2 The third element of the vector.
/// \return A vector containing the elements given as arguments.
///
/// \tparam T The contents type of the vector.
template<typename T>
inline std::vector<T> newvector(T x0, T x1, T x2) {
    std::vector<T> v(3);
    v[0] = x0;
    v[1] = x1;
    v[2] = x2;
    return v;
}

/// Create a vector with four element.
///
/// \param x0 The first element of the vector.
/// \param x1 The second element of the vector.
/// \param x2 The third element of the vector.
/// \param x3 The forth element of the vector.
/// \return A vector containing the elements given as arguments.
///
/// \tparam T The contents type of the vector.
template<typename T>
inline std::vector<T> newvector(T x0, T x1, T x2, T x3) {
    std::vector<T> v(4);
    v[0] = x0;
    v[1] = x1;
    v[2] = x2;
    v[3] = x3;
    return v;
}

/// Return the sign of a number a 1 (positive or zero) or -1 (negative).
///
/// \param a The number to determine the sign of.
/// \return The sign of the number as a value.
///
/// \tparam The type of the number and return value.
template<typename T>
inline T sign(T a) {
    return a>=0 ? 1 : -1;
}

/// Determine the maximum of two numbers.
///
/// \param a The first number.
/// \param b The second number.
/// \return The maximal value of a and b.
///
/// \tparam The type of the numbers and return value.
template<typename T>
inline T max(T a, T b) {
  return a>b ? a : b;
}

/// Determine the maximum of three numbers.
///
/// \param a The first number.
/// \param b The second number.
/// \param c The third number.
/// \return The maximal value of a, b and c.
///
/// \tparam The type of the numbers and return value.
template<typename T>
inline T max(T a, T b, T c) {
  return a>b ? ( a > c ? a : c) : ( b > c ? b : c);
}

/// Determine the minimum of two numbers.
///
/// \param a The first number.
/// \param b The second number.
/// \return The minimal value of a and b.
///
/// \tparam The type of the numbers and return value.
template<typename T>
inline T min(T a, T b) {
  return a<b ? a : b;
}

/// Cap a value at a given min and max values. That is,
/// \f[
///     y =
///        \left\{
///        \begin{array}{ll}
///           M & \textrm{if } x > M \\ \null
///           m & \textrm{if } x < m \\ \null
///           x & \textrm{otherwise}
///        \end{array}
///        \right.
/// \f]
/// where \f$ M \f$ is the max value, \f$ m \f$ the min value, \f$ x \f$ the
/// input and \f$ y \f$ the output.
///
/// \param value The value to be capped.
/// \param min The minimal allowed value.
/// \param max The maximal allowed value.
/// \return The value capped at min and max.
///
/// \tparam The type of the argument values and return value.
template<typename T>
inline T limit(T value, T min, T max) {
    if (value<min)
        return min;
    else if (value>max)
        return max;
    else
        return value;
}

/// Convert a value to a string using the output stream operator.
///
/// \param value The value to be converted to string
/// \return String representation of the value.
///
/// \tparam The type of the value.
template<typename T>
inline std::string to_string(T value) {
    std::ostringstream str_stream;
    str_stream << value;
    return str_stream.str();
}

/// Convert a vector of values a value to string representation using the
/// the output stream operator for strings implemented in Muninn.
///
/// \param value The value to be converted to string
/// \return String representation of the value.
///
/// \tparam The contents type of the vector. This type must have a associated
///         output stream operator.
template<typename T>
inline std::string to_string(const std::vector<T> &value) {
    std::ostringstream str_stream;
    Muninn::operator<<(str_stream,value);
    return str_stream.str();
}

/// Convert a string to a value using the input stream operator.
///
/// \param str The string to be converted.
/// \return The converted value.
///
/// \tparam The type that the string should be converted to.
template<typename T>
inline T from_string(std::string str) {
    std::istringstream str_stream(str);
    T value;
    str_stream >> value;
    return value;
}

/// Convert a string to a value using the input stream operator,
/// but setting the return value to a uninitialized value, in case
/// the operator fails.
///
/// \param str The string to be converted.
/// \param uninitialized_value This value is returned if the stream operator
///                            does not ouput any value.
/// \return The converted value.
///
/// \tparam The type that the string should be converted to.
template<typename T>
inline T from_string(std::string str, T uninitialized_value) {
    std::istringstream str_stream(str);
    T value = uninitialized_value;
    str_stream >> value;
    return value;
}

/// Split a string into tokens separated by white spaces.
///
/// \param str The string to be tokenized.
/// \param whitespaces Charters separating tokens.
/// \return A vector of tokens.
inline std::vector<std::string> tokenize(const std::string& str, const std::string& whitespaces = " \t") {
    std::vector<std::string> tokens;

    // Skip delimiters at beginning.
    std::string::size_type last_pos = str.find_first_not_of(whitespaces, 0);

    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(whitespaces, last_pos);

    while (pos != std::string::npos || last_pos != std::string::npos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(last_pos, pos-last_pos));

        // Skip delimiters.  Note the "not_of"
        last_pos = str.find_first_not_of(whitespaces, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(whitespaces, last_pos);
    }

    return tokens;
}

/// Return a copy of the string with leading and tailing white spaces removed.
///
/// \param str The string to be stripped.
/// \param whitespaces List of recognized white spaces.
/// \return A copy of the string stripped for leading and tailing white spaces.
inline std::string strip(const std::string& str, const std::string& whitespaces = " \t") {
    // Find first non-delimiter
    std::string::size_type first_pos = str.find_first_not_of(whitespaces);

    // Find last non-delimiter
    std::string::size_type last_pos = str.find_last_not_of(whitespaces);

    if (first_pos!=std::string::npos && last_pos!=std::string::npos)
        return str.substr(first_pos, last_pos-first_pos+1);
    else
        return "";
}

} // namespace Muninn

#endif // MUNINN_UTILS_H_

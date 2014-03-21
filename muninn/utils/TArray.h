// TArray.h
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

#ifndef MUNINN_TARRAY_H_
#define MUNINN_TARRAY_H_

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <limits>
#include <typeinfo>
#include <cstring>

#include "muninn/common.h"
#include "muninn/utils/utils.h"

#include "muninn/utils/TArrayBaseIterator.h"
#include "muninn/utils/TArrayFlatIterator.h"
#include "muninn/utils/TArrayFlatIteratorCoord.h"
#include "muninn/utils/TArrayReverseFlatIterator.h"
#include "muninn/utils/TArrayWhereTrueIterator.h"

#include "muninn/utils/TArrayMismatchSizeException.h"
#include "muninn/utils/TArrayReadErrorException.h"

namespace Muninn {

// Forward declarations
template<typename T> class TArray;
template<typename T> std::ostream &operator<<(std::ostream &output, const TArray<T> &array);
template<typename T> std::istream &operator>>(std::istream &input, TArray<T> &array);

// Type definitions
typedef TArray<double> DArray;        ///< Type definition of a TArray with double contents.
typedef TArray<int> IArray;           ///< Type definition of a TArray with integer contents.
typedef TArray<unsigned int> UArray;  ///< Type definition of a TArray with unsigned integer contents.
typedef TArray<bool> BArray;          ///< Type definition of a TArray with boolean contents.

// Types used in TArray
typedef unsigned int Index;           ///< The type used for indices in the array.
typedef unsigned int Dimension;       ///< The type used for the number of dimensions.

/// A general multidimensional that supports element-wise arithmetic operations.
///
/// \tparam T The type of the array contents.
template<typename T>
class TArray {
public:
    // Constructors
    TArray();
    TArray(Index dim1);
    TArray(Index dim1, Index dim2);
    TArray(const std::vector<Index> &newshape);
    TArray(const std::vector<Index> &newshape, T *storage);

    // Copy constructor
    TArray(const TArray<T> &right);

    // Copy constructor - for arbitrary array type
    template <typename T2>
    TArray(const TArray<T2> &right);

    // Destructor
    ~TArray();

    // Operators: getters and setters
    inline T& operator()(Index index);
    inline T& operator()(Index coord1, Index coord2);
    inline T& operator()(const std::vector<Index> &coord);
    template<typename TARRAY, typename U> inline T& operator()(const TArrayBaseIterator<TARRAY,U> &it);

    // Const operators: getters
    inline const T& operator()(Index index) const;
    inline const T& operator()(Index coord1, Index coord2) const;
    inline const T& operator()(const std::vector<Index> &coord) const;
    template<typename TARRAY, typename U> inline const T& operator()(const TArrayBaseIterator<TARRAY,U> &it) const;

    // Operators: assignments
    inline const TArray<T>& operator=(const T &right);
    inline const TArray<T>& operator=(const TArray<T> &right);

    // Operators: arithmetic assignments
    inline TArray<T>& operator+=(const T &right);
    inline TArray<T>& operator-=(const T &right);
    inline TArray<T>& operator*=(const T &right);
    inline TArray<T>& operator/=(const T &right);

    template<typename U> inline TArray<T>& operator+=(const TArray<U> &right) throw(TArrayMismatchSizeException);
    template<typename U> inline TArray<T>& operator-=(const TArray<U> &right) throw(TArrayMismatchSizeException);
    template<typename U> inline TArray<T>& operator*=(const TArray<U> &right) throw(TArrayMismatchSizeException);
    template<typename U> inline TArray<T>& operator/=(const TArray<U> &right) throw(TArrayMismatchSizeException);

    inline TArray<T>& operator+=(const TArray<T> &right) throw(TArrayMismatchSizeException);
    inline TArray<T>& operator-=(const TArray<T> &right) throw(TArrayMismatchSizeException);
    inline TArray<T>& operator*=(const TArray<T> &right) throw(TArrayMismatchSizeException);
    inline TArray<T>& operator/=(const TArray<T> &right) throw(TArrayMismatchSizeException);

    // Operators: unary arithmetic operators
    inline TArray<T> operator-() const;

    // Operators: binary arithmetic operators
    inline TArray<T> operator+(const T &right) const;
    inline TArray<T> operator-(const T &right) const;
    inline TArray<T> operator*(const T &right) const;
    inline TArray<T> operator/(const T &right) const;

    inline TArray<T> operator+(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<T> operator-(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<T> operator*(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<T> operator/(const TArray<T> &right) const throw(TArrayMismatchSizeException);

    // Operators: binary logical operators
    inline TArray<bool> operator<(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator<(const T &right) const;
    inline TArray<bool> operator>(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator>(const T &right) const;
    inline TArray<bool> operator<=(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator<=(const T &right) const;
    inline TArray<bool> operator>=(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator>=(const T &right) const;
    inline TArray<bool> operator==(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator==(const T &right) const;

    // Operators: binary logical operators
    inline TArray<bool> operator&&(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator||(const TArray<T> &right) const throw(TArrayMismatchSizeException);
    inline TArray<bool> operator!() const;

    // Setters
    inline void set_all_zero();

    // Getters
    inline std::vector<Index> get_shape() const;
    inline Index get_shape(Dimension dim) const;
    inline Dimension get_ndims() const;
    inline Index get_asize() const;
    inline const T* get_array() const;
    inline T* get_array();

    // Other mathematical functions
    inline T sum() const;
    inline T max() const;
    inline T min() const;
    inline T pow_sum(double exponent) const;
    inline T pow_sum(int exponent) const;

    inline T sum(const BArray &where) const throw(TArrayMismatchSizeException);
    inline T max(const BArray &where) const throw(TArrayMismatchSizeException);
    inline T min(const BArray &where) const throw(TArrayMismatchSizeException);

    // Shape related functions
    template<typename U> inline bool same_shape(const TArray<U> &right) const;
    inline bool has_shape(const std::vector<Index> &other_shape) const;
    inline bool nonempty() const;

    inline std::vector<Index> get_coord(Index index) const;
    inline Index get_index(const std::vector<Index> &coord) const;

    inline bool valid_coord(const std::vector<Index> &coord) const;

    // Functions for resizing
    TArray<T> extended(Index add_under, Index add_over) const;
    TArray<T> extended(Index add_under_1, Index add_under_2, Index add_over_1, Index add_over_2) const;
    TArray<T> extended(const std::vector<Index> &add_under, const std::vector<Index> &add_over) const;

    // Iterators
    typedef TArrayFlatIterator<TArray<T>, T> flatiterator;
    typedef TArrayReverseFlatIterator<TArray<T>, T> reverseflatiterator;
    typedef TArrayFlatIteratorCoord<TArray<T>, T> flatiteratorcoord;
    typedef TArrayWhereTrueIterator<TArray<T>, T> wheretrueiterator;

    flatiterator get_flatiterator();
    reverseflatiterator get_reverseflatiterator();
    flatiteratorcoord get_flatiteratorcoord();
    wheretrueiterator get_wheretrueiterator();

    // Const Iterators
    typedef TArrayFlatIterator<const TArray<T>, const T> constflatiterator;
    typedef TArrayReverseFlatIterator<const TArray<T>, const T> constreverseflatiterator;
    typedef TArrayFlatIteratorCoord<const TArray<T>, const T> constflatiteratorcoord;
    typedef TArrayWhereTrueIterator<const TArray<T>, const T> constwheretrueiterator;

    constflatiterator get_constflatiterator() const;
    constreverseflatiterator get_constreverseflatiterator() const;
    constflatiteratorcoord get_constflatiteratorcoord() const;
    constwheretrueiterator get_constwheretrueiterator() const;

    // Reading and writing methods
    inline std::ostream& write(std::ostream &output, int precision=-1, bool full_format=true, bool newlines=true) const;
    inline std::string write(int precision=-1, bool full_format=true, bool newlines=true) const;

    inline std::istream& read(std::istream &input, bool full_format=true);
    inline void read(const std::string &input, bool full_format=true);

    // Friends
    template<typename U> friend class TArray;
    friend std::ostream &operator<< <T> (std::ostream &output, const TArray<T> &array);
    friend std::istream &operator>> <T> (std::istream &input, TArray<T> &array);

protected:
    inline void reset_shape(const std::vector<Index> &newshape);

private:
    T *array;              ///< The a pointer to the internal array containing the actual contents
    Index asize;           ///< The size of the internal array.
    Dimension ndims;       ///< Number of dimensions for the TArray.
    Index *shape;          ///< The shape of the TArray.
    Index *stride;         ///< The distance between elements in each dimension in the internal array..
    bool array_ownership;  ///< Weather the object owns memory allocated for the internal array.

    // Private methods
    template<typename U> inline void duplicate_shape(const TArray<U> &right);
    template<typename U> void assert_same_size(const TArray<U> &other) const throw(TArrayMismatchSizeException);


    // Private getters used by the public const overloaded
    inline T& get(Index index) const;
    inline T& get(Index coord1, Index coord2) const;
    inline T& get(const std::vector<Index> &coord) const;
};

/// \fn TArray<T>::TArray()
/// Default constructor.
template<typename T>
TArray<T>::TArray() : array(NULL), asize(0), ndims(0), shape(NULL), stride(NULL), array_ownership(true) {}

/// Constructor for a 1-dimensional array.
///
/// \param dim1 The size of the first dimension.
template<typename T>
TArray<T>::TArray(Index dim1) : array(NULL), asize(dim1), ndims(1), shape(NULL), stride(NULL), array_ownership(true) {
    //assert(dim1>0);
    try {
        array = new T[asize];
        shape = new Index[ndims];
        stride = new Index[ndims];

    }
    catch (std::bad_alloc &ba) {
        delete[] array;
        delete[] shape;
        delete[] stride;
        throw;
    }

    shape[0] = dim1;
    stride[0] = 1;
    set_all_zero();
}

/// Constructor for a 2-dimensional array.
///
/// \param dim1 The size of the first dimension.
/// \param dim2 The size of the second dimension.
template<typename T>
TArray<T>::TArray(Index dim1, Index dim2) : array(NULL), asize(dim1*dim2), ndims(2), shape(NULL), stride(NULL), array_ownership(true) {
    //assert(dim1>0 && dim2>0);
    try {
        array = new T[asize];
        shape = new Index[ndims];
        stride = new Index[ndims];
    }
    catch (std::bad_alloc &ba) {
        delete[] array;
        delete[] shape;
        delete[] stride;
        throw;
    }

    shape[0] = dim1;
    shape[1] = dim2;

    stride[0] = 1;
    stride[1] = dim1;

    set_all_zero();
}

/// Constructor for a multidimensional array.
///
/// \param newshape The shape of the array.
template<typename T>
TArray<T>::TArray(const std::vector<Index> &newshape) : array(NULL), asize(1), ndims(newshape.size()), shape(NULL), stride(NULL), array_ownership(true) {
    assert(newshape.size()>0);

    try {
        shape = new Index[ndims];
        stride = new Index[ndims];
    }
    catch (std::bad_alloc &ba) {
        delete[] shape;
        delete[] stride;
        throw;
    }

    for (Dimension dim = 0; dim < ndims; dim++) {
        shape[dim] = newshape[dim];
        stride[dim] = asize;         // NOTE: These two line cannot be swapped!
        asize *= newshape[dim];      // NOTE: These two line cannot be swapped!
    }

    try {
        array = new T[asize];
    }
    catch (std::bad_alloc &ba) {
        delete[] array;
        delete[] shape;
        delete[] stride;
        throw;
    }

    set_all_zero();
}

/// Construct an TArray that wraps a normal C-style array. Note that the
/// TArray does not take ownership of the C-array, and accordingly it does not
/// delete it.
///
/// \param newshape The shape of the array.
/// \param storage The C-style array that is to be wrapped.
template<typename T>
TArray<T>::TArray(const std::vector<Index> &newshape, T *storage) : array(storage), asize(1), ndims(newshape.size()), shape(NULL), stride(NULL), array_ownership(false) {
    assert(newshape.size()>0);

    try {
        shape = new Index[ndims];
        stride = new Index[ndims];
    }
    catch (std::bad_alloc &ba) {
        delete[] shape;
        delete[] stride;
        throw;
    }

    for (Dimension dim = 0; dim < ndims; dim++) {
        shape[dim] = newshape[dim];
        stride[dim] = asize;         // NOTE: These two line cannot be swapped!
        asize *= newshape[dim];      // NOTE: These two line cannot be swapped!
    }
}


/// Copy constructor.
///
/// \param right The array to be copied.
template<typename T>
TArray<T>::TArray(const TArray<T> &right) : array(NULL), asize(0), ndims(0), shape(NULL), stride(NULL), array_ownership(true) {
    duplicate_shape(right);
    for (Index i = 0; i < asize; i++)
        array[i] = right.array[i];
}

/// Copy constructor for for arbitrary array type
///
/// \param right The array to be copied.
template<typename T1>
template<typename T2>
TArray<T1>::TArray(const TArray<T2> &right) : array(NULL), asize(0), ndims(0), shape(NULL), stride(NULL), array_ownership(true) {
    duplicate_shape(right);
    for (Index i = 0; i < asize; i++)
        array[i] = right.array[i];
}

/// \fn TArray<T>::~TArray()
/// Default destructor. Note that the destructor check for ownership of
/// the internal array.
template<typename T>
TArray<T>::~TArray() {
    if (array_ownership)
        delete[] array;
    delete[] shape;
    delete[] stride;
}

/// Access an element in a 1-dimensional array.
///
/// \param index The index of the elements.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::operator()(Index index) {
    return get(index);
}

/// Access an element in a 2-dimensional array.
///
/// \param coord1 The coordinate (index) in the first dimension.
/// \param coord2 The coordinate (index) in the second dimension.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::operator()(Index coord1, Index coord2) {
    return get(coord1, coord2);
}

/// Access an element in a multidimensional array.
///
/// \param coord The coordinate of the element.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::operator()(const std::vector<Index> &coord) {
    return get(coord);
}

/// Access an element in the array using an iterator.
///
/// \param it An iterator pointing to an element in the array.
/// \return The value corresponding to the index of the iterator.
template<typename T>
template<typename TARRAY, typename U>
inline T& TArray<T>::operator()(const TArrayBaseIterator<TARRAY,U> &it) {
    return get(it.get_index());
}

/// Get a element in a constant 1-dimensional array.
///
/// \param index The index of the elements.
/// \return The value with the given coordinate.
template<typename T>
inline const T& TArray<T>::operator()(Index index) const {
    return get(index);
}

/// Get an element in a constant 2-dimensional array.
///
/// \param coord1 The coordinate (index) in the first dimension.
/// \param coord2 The coordinate (index) in the second dimension.
/// \return The value with the given coordinate.
template<typename T>
inline const T& TArray<T>::operator()(Index coord1, Index coord2) const {
    return get(coord1, coord2);
}

/// Get an element in a constant multidimensional array.
///
/// \param coord The coordinate of the element.
/// \return The value with the given coordinate.
template<typename T>
inline const T& TArray<T>::operator()(const std::vector<Index> &coord) const {
    return get(coord);
}

/// Get an element in a constant array using an iterator.
///
/// \param it An iterator pointing to an element in the array.
/// \return The value corresponding to the index of the iterator.
template<typename T>
template<typename TARRAY,typename U>
inline const T& TArray<T>::operator()(const TArrayBaseIterator<TARRAY,U> &it) const {
    return get(it.get_index());
}

/// Assign a value to all elements in the array.
///
/// \param right The value to be assigned.
template<typename T>
inline const TArray<T>& TArray<T>::operator=(const T &right) {
    for (Index i=0; i<asize; i++) {
        array[i] = right;
    }
    return *this;
}

/// Assign this array all the values of an other array. If necessary this
/// array will be resized.
///
/// \param right The array values to be assigned.
template<typename T>
inline const TArray<T>& TArray<T>::operator=(const TArray<T> &right) {
    // Check for self-assignment
    if (&right != this) {
        // Check if they have same shape and copy shape if they differ
        if (!same_shape(right)) {
            // Delete the old arrays
            if (array_ownership)
                delete[] array;
            delete[] shape;
            delete[] stride;

            // Copy the new values
            duplicate_shape(right);
        }

        // Copy data
        for (Index i = 0; i < asize; i++)
            array[i] = right.array[i];
    }
    return *this;
}

/// Add a value to all elements in the array.
///
/// \param right The value to be added.
template<typename T>
inline TArray<T>& TArray<T>::operator+=(const T &right) {
    for (Index i = 0; i < asize; i++) {
        array[i] += right;
    }
    return *this;
}

/// Subtract a value from all elements in the array.
///
/// \param right The value to be subtracted.
template<typename T>
inline TArray<T>& TArray<T>::operator-=(const T &right) {
    for (Index i = 0; i < asize; i++) {
        array[i] -= right;
    }
    return *this;
}

/// Multiply all elements in the array with a value.
///
/// \param right The value to be multiplied by.
template<typename T>
inline TArray<T>& TArray<T>::operator*=(const T &right) {
    for (Index i = 0; i < asize; i++) {
        array[i] *= right;
    }
    return *this;
}

/// Divide all elements in the array with a value.
///
/// \param right The value to be divided by.
template<typename T>
inline TArray<T>& TArray<T>::operator/=(const T &right) {
    for (Index i = 0; i < asize; i++) {
        array[i] /= right;
    }
    return *this;
}

/// Add the values of another array to the values of this array element-wise.
/// The arrays must have the same size, but the other array may have a
/// different contents type.
///
/// \param right The array to be element-wise added to this array.
/// \return A reference to this array.
template<typename T>
template<typename U>
inline TArray<T>& TArray<T>::operator+=(const TArray<U> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] += static_cast<T>(right.array[i]);
    }
    return *this;
}

/// Subtract the value of another array to the values of this array element-wise.
/// The arrays must have the same size, but the other array may have a
/// different contents type.
///
/// \param right The array to be element-wise subtracted from this array.
/// \return A reference to this array.
template<typename T>
template<typename U>
inline TArray<T>& TArray<T>::operator-=(const TArray<U> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] -= static_cast<T>(right.array[i]);
    }
    return *this;
}

/// Multiply the value of this array by the value of another array element-wise.
/// The two arrays must have the same size, but the other array may have a
/// different contents type.
///
/// \param right The array this array is element-wise multiplied by.
/// \return A reference to this array.
template<typename T>
template<typename U>
inline TArray<T>& TArray<T>::operator*=(const TArray<U> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] *= static_cast<T>(right.array[i]);
    }
    return *this;
}

/// Divide the values of this array by the values of another array element-wise.
/// The two arrays must have the same size, but the other array may have a
/// different contents type.
///
/// \param right The array this array is element-wise divided by.
/// \return A reference to this array.
template<typename T>
template<typename U>
inline TArray<T>& TArray<T>::operator/=(const TArray<U> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] /= static_cast<T>(right.array[i]);
    }
    return *this;
}

/// Add the values of another array to the values of this array element-wise.
/// The arrays must have the same size and contents type.
///
/// \param right The array to be element-wise added to this array.
/// \return A reference to this array.
template<typename T>
inline TArray<T>& TArray<T>::operator+=(const TArray<T> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] += right.array[i];
    }
    return *this;
}

/// Subtract the value of another array to the values of this array element-wise.
/// The arrays must have the same size and contents type.
///
/// \param right The array to be element-wise subtracted from this array.
/// \return A reference to this array.
template<typename T>
inline TArray<T>& TArray<T>::operator-=(const TArray<T> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] -= right.array[i];
    }
    return *this;
}

/// Multiply the value of this array by the value of another array element-wise.
/// The two arrays must have the same size and contents size.
///
/// \param right The array this array is element-wise multiplied by.
/// \return A reference to this array.
template<typename T>
inline TArray<T>& TArray<T>::operator*=(const TArray<T> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] *= right.array[i];
    }
    return *this;
}

/// Divide the values of this array by the values of another array element-wise.
/// The two arrays must have the same size and contents type.
///
/// \param right The array this array is element-wise divided by.
/// \return A reference to this array.
template<typename T>
inline TArray<T>& TArray<T>::operator/=(const TArray<T> &right) throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    for (Index i = 0; i < asize; i++) {
        array[i] /= right.array[i];
    }
    return *this;
}

/// Get the element-wise negative of this array.
///
/// \return The element-wise negative of this array.
template<typename T>
inline TArray<T> TArray<T>::operator-() const {
    TArray<T> result(*this);
    for (Index i = 0; i < result.asize; i++) {
        result.array[i] = -result.array[i];
    }
    return result;
}

/// Element-wise addition of the element of the array and a single value.
///
/// \param right The right hand side of the addition.
/// \return The element-wise sums of the array and the value.
template<typename T>
inline TArray<T> TArray<T>::operator+(const T &right) const {
    TArray<T> result(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] += right;
    }
    return result;
}

/// Element-wise subtraction of the element of the array and a single value.
///
/// \param right The right hand side of the subtraction.
/// \return The element-wise differences of the array and the value.
template<typename T>
inline TArray<T> TArray<T>::operator-(const T &right) const {
    TArray<T> result(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] -= right;
    }
    return result;
}

/// Element-wise multiplication of the element of the array and a single value
///
/// \param right The right hand side of the multiplication.
/// \return The element-wise products of the array and the value.
template<typename T>
inline TArray<T> TArray<T>::operator*(const T &right) const {
    TArray<T> result(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] *= right;
    }
    return result;
}

/// Element-wise division of the element of the array and a single value
///
/// \param right The right hand side of the division.
/// \return The element-wise quotients of the array and the value.
template<typename T>
inline TArray<T> TArray<T>::operator/(const T &right) const {
    TArray<T> result(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] /= right;
    }
    return result;
}

/// Element-wise addition of two array.
///
/// \param right The right hand side of the addition.
/// \return The element-wise sums of two arrays.
template<typename T>
inline TArray<T> TArray<T>::operator+(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);

    TArray<T> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] + right.array[i];
    }
    return result;
}

/// Element-wise subtraction of two array.
///
/// \param right The right hand side of the subtraction.
/// \return The element-wise differences of two arrays.
template<typename T>
inline TArray<T> TArray<T>::operator-(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);

    TArray<T> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] - right.array[i];
    }
    return result;
}

/// Element-wise multiplication of two array.
///
/// \param right The right hand side of the multiplication.
/// \return The element-wise products of two arrays.
template<typename T>
inline TArray<T> TArray<T>::operator*(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);

    TArray<T> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] * right.array[i];
    }
    return result;
}

/// Element-wise division of two array.
///
/// \param right The right hand side of the division.
/// \return The element-wise quotients of two arrays.
template<typename T>
inline TArray<T> TArray<T>::operator/(const TArray<T> &right) const throw(TArrayMismatchSizeException){
    assert_same_size(right);

    TArray<T> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] / right.array[i];
    }
    return result;
}

/// Element-wise comparison (<) of two arrays. The arrays must have the same size.
///
/// \param right The right hand side of the comparison.
/// \return The outcome of the element-wise comparison of the two array.
template<typename T>
inline TArray<bool> TArray<T>::operator<(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] < right.array[i];
    }
    return result;
}

/// Compare (<) all the elements of the array to a value.
///
/// \param right The right hand side value to compare to.
/// \return The outcome of the element-wise comparison of the array and the value.
template<typename T>
inline TArray<bool> TArray<T>::operator<(const T &right) const {
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] < right;
    }
    return result;
}

/// Element-wise comparison (>) of two arrays. The arrays must have the same size.
///
/// \param right The right hand side of the comparison.
/// \return The outcome of the element-wise comparison of the two array.
template<typename T>
inline TArray<bool> TArray<T>::operator>(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] > right.array[i];
    }
    return result;
}

/// Compare (>) all the elements of the array to a value.
///
/// \param right The right hand side value to compare to.
/// \return The outcome of the element-wise comparison of the array and the value.
template<typename T>
inline TArray<bool> TArray<T>::operator>(const T &right) const {
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] > right;
    }
    return result;
}

/// Element-wise comparison (<=) of two arrays. The arrays must have the same size.
///
/// \param right The right hand side of the comparison.
/// \return The outcome of the element-wise comparison of the two array.
template<typename T>
inline TArray<bool> TArray<T>::operator<=(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] <= right.array[i];
    }
    return result;
}

/// Compare (<=) all the elements of the array to a value.
///
/// \param right The right hand side value to compare to.
/// \return The outcome of the element-wise comparison of the array and the value.
template<typename T>
inline TArray<bool> TArray<T>::operator<=(const T &right) const {
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] <= right;
    }
    return result;
}

/// Element-wise comparison (>=) of two arrays. The arrays must have the same size.
///
/// \param right The right hand side of the comparison.
/// \return The outcome of the element-wise comparison of the two array.
template<typename T>
inline TArray<bool> TArray<T>::operator>=(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] >= right.array[i];
    }
    return result;
}

/// Compare (>=) all the elements of the array to a value.
///
/// \param right The right hand side value to compare to.
/// \return The outcome of the element-wise comparison of the array and the value.
template<typename T>
inline TArray<bool> TArray<T>::operator>=(const T &right) const {
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] >= right;
    }
    return result;
}

/// Element-wise comparison (==) of two arrays. The arrays must have the same size.
///
/// \param right The right hand side of the comparison.
/// \return The outcome of the element-wise comparison of the two array.
template<typename T>
inline TArray<bool> TArray<T>::operator==(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] == right.array[i];
    }
    return result;
}

/// Compare (==) all the elements of the array to a value.
///
/// \param right The right hand side value to compare to.
/// \return The outcome of the element-wise comparison of the array and the value.
template<typename T>
inline TArray<bool> TArray<T>::operator==(const T &right) const {
    TArray<bool> result;
    result.duplicate_shape(*this);
    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] == right;
    }
    return result;
}

/// Perform the binary logical operator && element-wise on two arrays.
///
/// \param right The right hand side of the binary operator.
/// \return The outcome of the element-wise logical operator.
template<typename T>
inline TArray<bool> TArray<T>::operator&&(const TArray<T> &right) const throw(TArrayMismatchSizeException){
    assert_same_size(right);

    TArray<bool> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] && right.array[i];
    }
    return result;
}

/// Perform the binary logical operator || element-wise on two arrays.
///
/// \param right The right hand side of the binary operator.
/// \return The outcome of the element-wise logical operator.
template<typename T>
inline TArray<bool> TArray<T>::operator||(const TArray<T> &right) const throw(TArrayMismatchSizeException) {
    assert_same_size(right);

    TArray<bool> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = array[i] || right.array[i];
    }
    return result;
}

/// Use the unary logical operator ! element wise to the array.
///
/// \return The outcome of applying the unary operator element-wise to the array.
template<typename T>
inline TArray<bool> TArray<T>::operator!() const {
    TArray<bool> result;
    result.duplicate_shape(*this);

    for (Index i = 0; i < asize; i++) {
        result.array[i] = !array[i];
    }
    return result;
}

/// Set all elements of the array to zero.
template<typename T>
inline void TArray<T>::set_all_zero() {
    for (Index i = 0; i < asize; i++) {
        array[i] = 0;
    }
}

/// Get the shape of the array.
///
/// \return The shape of the array.
template<typename T>
inline std::vector<Index> TArray<T>::get_shape() const {
    std::vector<Index> newshape(ndims);
    for (Dimension dim = 0; dim < ndims; dim++) {
        newshape[dim] = shape[dim];
    }
    return newshape;
}

/// Get the shape of the array in a given dimension.
///
/// \param dim The dimension to get the shape at.
/// \return The shape in the given dimension.
template<typename T>
inline Index TArray<T>::get_shape(Dimension dim) const {
    assert(0<=dim && dim<ndims);
    return shape[dim];
}

/// Get the number of dimensions.
///
/// \return Number of dimensions.
template<typename T>
inline Dimension TArray<T>::get_ndims() const {
    return ndims;
}

/// Get the size of the internal array.
///
/// \return The size of the internal array.
template<typename T>
inline Index TArray<T>::get_asize() const {
    return asize;
}

/// Get a constant pointer to the internal array. Use this function with care,
/// several other function may invalidate the pointer.
///
/// \return A constant pointer to the internal array.
template<typename T>
inline const T* TArray<T>::get_array() const {
    return array;
}

/// Get a pointer to the internal array. Use this function with care,
/// several other function may invalidate the pointer.
///
/// \return A pointer to the internal array.
template<typename T>
inline T* TArray<T>::get_array() {
    return array;
}

/// Calculate the sum of all elements in the array.
///
/// \return The sum of all elements in the array.
template<typename T>
inline T TArray<T>::sum() const {
    T SUM = 0;
    for (Index i = 0; i < asize; i++)
        SUM += array[i];
    return SUM;
}

/// Returns the maximal element in the array. Will return minus infinity if the
/// array is empty.
///
/// \return The maximal element of the array.
template<typename T>
inline T TArray<T>::max() const {
    T MAX = -std::numeric_limits<T>::infinity();
    for (Index i = 0; i < asize; i++)
        if (array[i] > MAX)
            MAX = array[i];
    return MAX;
}

/// Returns the minimal element in the array. Will return infinity if the
/// array is empty.
///
/// \return The minimal element of the array.
template<typename T>
inline T TArray<T>::min() const {
    T MIN = std::numeric_limits<T>::infinity();
    for (Index i = 0; i < asize; i++)
        if (array[i] < MIN)
            MIN = array[i];
    return MIN;
}

/// Returns the sum of all elements in the array, where each element is
/// raised to a integer type power.
///
/// \param exponent The integer exponent of the power function.
/// \return The sum of all elements, where each elemt in reaised to a given power.
template<typename T>
inline T TArray<T>::pow_sum(double exponent) const {
    T SUM = 0;
    for (Index i = 0; i < asize; i++)
        SUM += std::pow(array[i], exponent);
    return SUM;
}

/// Returns the some of all elements in the array, where each element is
/// raised to a double type power.
///
/// \param exponent The double type exponent of the power function.
/// \return The sum of all elements, where each elemt in reaised to a given power.
template<typename T>
inline T TArray<T>::pow_sum(int exponent) const {
    T SUM = 0;
    for (Index i = 0; i < asize; i++)
        SUM += std::pow(array[i], exponent);
    return SUM;
}

/// Calculate the sum of elements elements in the array using a
/// limited set of indices.
///
/// \param where The sum only runs over indices, where this array is true.
/// \return The sum of elements where the where-array is true.
template<typename T>
inline T TArray<T>::sum(const BArray &where) const throw(TArrayMismatchSizeException) {
    assert_same_size(where);
    T SUM = 0;
    for (Index i = 0; i < asize; i++)
        if (where.array[i])
            SUM += array[i];
    return SUM;
}

/// Find the maximal value in the array among a limited set of indices. Will
/// return minus infinity if the array is empty.
///
/// \param where Only indices where this array is true is considered.
/// \return The maximal value in the array over a limited set of indices.
template<typename T>
inline T TArray<T>::max(const BArray &where) const throw(TArrayMismatchSizeException) {
    assert_same_size(where);
    T MAX = -std::numeric_limits<T>::infinity();
    for (Index i = 0; i < asize; i++)
        if (array[i] > MAX && where.array[i])
            MAX = array[i];
    return MAX;
}

/// Find the minimal value in the array among a limited set of indices. Will
/// return infinity if the array is empty.
///
/// \param where Only indices where this array is true is considered.
/// \return The minimal value in the array over a limited set of indices.
template<typename T>
inline T TArray<T>::min(const BArray &where) const throw(TArrayMismatchSizeException) {
    assert_same_size(where);
    T MIN = std::numeric_limits<T>::infinity();
    for (Index i = 0; i < asize; i++)
        if (array[i] < MIN && where.array[i])
            MIN = array[i];
    return MIN;
}

/// Determines if this array has the same shape as another array.
///
/// \param right The array to compare with.
/// \return Returns true only if the arrays has the same shape-
template<typename T>
template<typename U>
inline bool TArray<T>::same_shape(const TArray<U> &right) const {
    bool same = true;

    if (ndims != right.ndims) {
        same = false;
    } else {
        for (Dimension dim = 0; dim < ndims; dim++) {
            if (shape[dim] != right.shape[dim]) {
                same = false;
            }
        }
    }

    return same;
}

/// Determines if an array has a given shape.
///
/// \param other_shape Check if the array has this shape.
/// \return Returns true only if the array has given shape.
template<typename T>
inline bool TArray<T>::has_shape(const std::vector<Index> &other_shape) const {
    bool same = true;

    if (ndims != other_shape.size()) {
        same = false;
    } else {
        for (Dimension dim = 0; dim < ndims; dim++) {
            if (shape[dim] != other_shape[dim]) {
                same = false;
            }
        }
    }

    return same;
}

/// Determines if the array is non-empty.
///
/// \return Returns true only if the array is non-empty.
template<typename T>
inline bool TArray<T>::nonempty() const {
    return asize>0;
}

/// Calculate the coordinate corresponding to a given index in the internal array.
///
/// \param index A index in the internal array.
/// \return Returns the coordinate corresponding to the index.
template<typename T>
inline std::vector<Index> TArray<T>::get_coord(Index index) const {
    assert(index<asize);
    std::vector<Index> coord(ndims, 0);
    for (int dim=coord.size()-1; dim>=0; dim--) {
        coord[dim] = index / stride[dim];
        index -= coord[dim] * stride[dim];
    }
    return coord;
}

/// Calculate the index in the internal array corresponding to a given coordinate.
///
/// \param coord A coordinate in the array.
/// \return Returns the index corresponding to the coordinate.
template<typename T>
inline Index TArray<T>::get_index(const std::vector<Index> &coord) const {
    Index index = 0;
    for (Dimension dim=0; dim<coord.size(); dim++) {
        index += coord[dim]*stride[dim];
    }
    return index;
}

/// Check if a coordinate is value for the array.
///
/// \param coord A coordinate.
/// \return Returns only true if the coordiante is valid.
template<typename T>
inline bool TArray<T>::valid_coord(const std::vector<Index> &coord) const {
    return get_index(coord) < asize;
}

/// Returns an extend (resize) version of a 1-dimensional array.
///
/// \param add_under The number of bins to add below index 0.
/// \param add_over The number of bins to add above the size of the array.
/// \return An extend version of the array.
template<typename T>
TArray<T> TArray<T>::extended(Index add_under, Index add_over) const {
    // Note, that since arguments are unsigned (Index should be unsigned), they cannot be negative
    // Assert that he array is one dimensional
    assert(ndims==1);

    // Calculate the shape of the new array
    Index new_dim_1 = add_under + asize + add_over;

    // Create the new array
    TArray<T> newarray(new_dim_1);

    // Copy the content from the original to the new
    for(Index i=0; i<asize; i++) {
        newarray.array[add_under+i] = array[i];
    }

    return newarray;
}

/// Returns an extend (resize) version of a 2-dimensional array.
///
/// \param add_under_1 The number of bins to add below index 0 in the first dimension.
/// \param add_under_2 The number of bins to add below index 0 in the second dimension.
/// \param add_over_1 The number of bins to add above the size of the array in the first dimension.
/// \param add_over_2 The number of bins to add above the size of the array in the second dimension.
/// \return An extend version of the array.
template<typename T>
TArray<T> TArray<T>::extended(Index add_under_1, Index add_under_2, Index add_over_1, Index add_over_2) const {
    // Note, that since arguments are unsigned (Index should be unsinged), they cannot be negative
    // Assert that he array is one dimensional
    assert(ndims==2);

    // Calculate the shape of the new array
    Index new_dim_1 = add_under_1 + shape[0] + add_over_1;
    Index new_dim_2 = add_under_2 + shape[1] + add_over_2;

    // Create the new array
    TArray<T> newarray(new_dim_1, new_dim_2);

    // Copy the content from the original to the new
    for (Index cord2 = 0; cord2 < shape[1]; cord2++) {
        for (Index cord1 = 0; cord1 < shape[0]; cord1++) {
            newarray(cord1+add_under_1, cord2+add_under_2) = (*this)(cord1,cord2);
        }
    }

    return newarray;
}

/// Returns an extend (resize) version of a multidimensional array.
///
/// \param add_under The number of bins to add in each dimension below index 0.
/// \param add_over The number of bins to add in each dimension above the array size in the given dimension.
/// \return An extend version of the array.
template<typename T>
TArray<T> TArray<T>::extended(const std::vector<Index> &add_under, const std::vector<Index> &add_over) const {
    // Note, that since arguments are unsigned (Index should be unsigned), they cannot be negative
    // Assert that he array is one dimensional
    assert(add_under.size() == ndims);
    assert(add_over.size() == ndims);

    switch (ndims) {
    case 1:
        return extended(add_under[0], add_over[0]);
    case 2:
        return extended(add_under[0], add_under[1], add_over[0], add_over[1]);
    default:
        // Calculate the shape of the new array
        std::vector<Index> newshape = add_vectors(add_under, add_over, get_shape());

        // Create the new array
        TArray<T> newarray(newshape);

        // Copy the content from the original to the new
        for(constflatiteratorcoord it = get_constflatiteratorcoord(); it(); ++it) {
            newarray(add_vectors(add_under, it.get_coord())) = *it;
        }

        return newarray;
    }
}

/// Get a flat iterator over the array.
///
/// \return A flat iterator.
template<typename T>
TArrayFlatIterator<TArray<T>,T> TArray<T>::get_flatiterator() {
    return TArrayFlatIterator<TArray<T>,T>(*this);
}

/// Get a reverse flat iterator over the array.
///
/// \return A reverse flat iterator.
template<typename T>
TArrayReverseFlatIterator<TArray<T>,T> TArray<T>::get_reverseflatiterator() {
    return TArrayReverseFlatIterator<TArray<T>,T>(*this);
}

/// Get a flat iterator with coordinates over the array.
///
/// \return A reverse flat iterator with coordinates.
template<typename T>
TArrayFlatIteratorCoord<TArray<T>,T> TArray<T>::get_flatiteratorcoord() {
    return TArrayFlatIteratorCoord<TArray<T>,T>(*this);
}

/// Get a iterator over the elements that are true in the array.
///
/// \return A iterator over true elements.
template<typename T>
TArrayWhereTrueIterator<TArray<T>,T> TArray<T>::get_wheretrueiterator() {
    return TArrayWhereTrueIterator<TArray<T>,T>(*this);
}

/// Get a constant flat iterator over the array.
///
/// \return A constant flat iterator.
template<typename T>
TArrayFlatIterator<const TArray<T>,const T> TArray<T>::get_constflatiterator() const {
    return TArrayFlatIterator<const TArray<T>,const T>(*this);
}

/// Get a constant reverse flat iterator over the array.
///
/// \return A constant reverse flat iterator.
template<typename T>
TArrayReverseFlatIterator<const TArray<T>,const T> TArray<T>::get_constreverseflatiterator() const {
    return TArrayReverseFlatIterator<const TArray<T>,const T>(*this);
}

/// Get a constant flat iterator with coordinates over the array.
///
/// \return A constant reverse flat iterator with coordinates.
template<typename T>
TArrayFlatIteratorCoord<const TArray<T>,const T> TArray<T>::get_constflatiteratorcoord() const {
    return TArrayFlatIteratorCoord<const TArray<T>,const T>(*this);
}

/// Get a constant iterator over the elements that are true in the array.
///
/// \return A constant iterator over true elements.
template<typename T>
TArrayWhereTrueIterator<const TArray<T>,const T> TArray<T>::get_constwheretrueiterator() const {
    return TArrayWhereTrueIterator<const TArray<T>,const T>(*this);
}

/// Change the shape of an array. This is done by deleting the internal array
/// and all other information regarding the array.
///
/// \param newshape The new shape of the array.
template<typename T>
inline void TArray<T>::reset_shape(const std::vector<Index> &newshape) {
    assert(newshape.size()>0);

    if (array_ownership)
        delete[] array;
    delete[] shape;
    delete[] stride;

    array=NULL;
    shape=NULL;
    stride=NULL;

    ndims = newshape.size();

    try {
        shape = new Index[ndims];
        stride = new Index[ndims];
    }
    catch (std::bad_alloc &ba) {
        delete[] shape;
        delete[] stride;
        throw;
    }

    asize = 1;
    for (Dimension dim = 0; dim < ndims; dim++) {
        shape[dim] = newshape[dim];
        stride[dim] = asize;         // NOTE: These two line cannot be swapped!
        asize *= newshape[dim];      // NOTE: These two line cannot be swapped!
    }

    try {
        array = new T[asize];
    }
    catch (std::bad_alloc &ba) {
        delete[] shape;
        delete[] stride;
        delete[] array;
        throw;
    }

    set_all_zero();
}

/// Set the shape of the array to be the same as another array.
///
/// Note that the internal array and additional arrays are not freed.
/// Accordingly the array should be empty, when this method is called.
///
/// \param right The array to obtain the shape from.
///
/// \tparam U The type of the contents for the other array.
template<typename T>
template<typename U>
inline void TArray<T>::duplicate_shape(const TArray<U> &right) {
    // NOTE THAT ARRAY AND SHAPE IS NOT FREED
    ndims = right.ndims;
    asize = right.asize;

    if (ndims > 0) {
         shape = new Index[ndims];
        stride = new Index[ndims];
    }
    else {
        shape = NULL;
        stride = NULL;
    }

    if (right.array != NULL)
        array = new T[asize];
    else
        array = NULL;

    for (Dimension dim = 0; dim < ndims; dim++) {
        shape[dim] = right.shape[dim];
        stride[dim] = right.stride[dim];
    }
}

/// Check if the array has the same size as another array. If they do note have
/// the same size a TArrayMismatchSizeException exception is thrown.
///
/// \param other The array to compare shape with.s
///
/// \tparam U The type of the contents for the other array.
template<typename T>
template<typename U>
void TArray<T>::assert_same_size(const TArray<U> &other) const throw(TArrayMismatchSizeException) {
    if (asize!=other.asize)
        throw TArrayMismatchSizeException(asize, other.asize);
}

/// Function for writing the array to an output stream.
///
/// \param output The output stream to write to.
/// \param precision The number of significant digits used when writing floating point values.
/// \param full_format Whether the full format (including the shape of the array) should be used.
/// \param newlines Whether newlines should be used for formatting the output.
/// \return A reference to the output stream.
template<typename T>
inline std::ostream& TArray<T>::write(std::ostream &output, int precision, bool full_format, bool newlines) const {
    // Save the old precision and the the new
    std::streamsize old_precision = output.precision();

    if (precision>=0)
        output.precision(precision);

    // Output that it's an TArray
    if (full_format)
        output << "TArray(";

    // Output first opening bracket
    output << "[";

    // Output contents
    Dimension dim = ndims;

    for (Index i = 0; i < asize; i++) {
        // Output opening brackets
        if(dim>1) {
            if(i>0 && newlines) {
                if(full_format)
                    output << "\n        ";
                else
                    output << "\n ";
            }
            for(Dimension dim=ndims-1; dim>0; dim--) {
                if(i % stride[dim] == 0)
                    output << "[";
                else if(newlines)
                    output << " ";
            }
        }

        // Output data point
        output << array[i];

        // Output closing brackets
        for(dim=1; dim<ndims; dim++) {
            if( (i+1) % stride[dim] == 0)
                output << "]";
            else {
                break;
            }
        }

        // Output a space between the numbers
        if(dim==1 && i<(asize-1))
            output << " ";

    }

    // Output last closing bracket
    output << "]";

    if (full_format) {
        output << ", type=" << typeid(T).name() << ", shape=[";

        for(Dimension dim=0; dim<ndims; dim++)
            output << shape[dim] << (dim<ndims-1 ? " " : "");

        output << "])";
    }

    // Set back the precision
    if (precision>=0)
        output.precision(old_precision);

    return output;
}

/// Function for converting the array to string representation.
///
/// \param precision The number of significant digits used when writing floating point values.
/// \param full_format Whether the full format (including the shape of the array) should be used.
/// \param newlines Whether newlines should be used for formatting the output.
/// \return A representation string of the array.
template<typename T>
inline std::string TArray<T>::write(int precision, bool full_format, bool newlines) const {
    std::ostringstream tmp;
    write(tmp, precision, full_format, newlines);
    return tmp.str();
}

/// Function for reading an array from a input stream.
///
/// \param input The steam to read from.
/// \param full_format Whether the string representation is in the full format (including the shape of the array).
/// \return A reference to the input stream.
template<typename T>
inline std::istream& TArray<T>::read(std::istream &input, bool full_format) {
    // In full_format the first text should be the "TArray(" token
    // Read 8 characters (7 + \0)
    if (full_format) {
        // Skip whitespaces
        while(input.peek() == ' ')
            input.ignore(1);

        char s[8];
        input.get(s, 8);

        if (strcmp(s, "TArray(") != 0 || !input.good())
            throw TArrayReadErrorException("The deceleration 'TArray' is missing in the beginning of the read.");
    }

    // Read the data
    std::vector<T> data;
    int open_brackets = 0;

    do {
        // Skip whitespaces and newlines
        while(input.peek() == ' ' || input.peek() == '\n')
            input.ignore(1);

        // Read the next depending on if it's a bracket or a data point
        if (input.peek() == '[') {
            input.ignore(1);
            ++open_brackets;
        }
        else if (input.peek() == ']') {
            input.ignore(1);
            --open_brackets;
        }
        else if (input.good()){
            T datapoint;
            input >> datapoint;
            data.push_back(datapoint);
        }

    } while (open_brackets > 0 && input.good());

    // Check that there was enough closing brackets
    if (open_brackets > 0)
        throw TArrayReadErrorException("Reached end of input, but not all brackets were closed.");

    // In full format, read the shape
    if (full_format) {
        std::vector<Index> new_shape;

        do {
            // Read until the next comma or closing parenthesis
            std::ostringstream reading;
            while(input.peek() != ',' && input.peek() != ')' && input.good())
                reading.put(input.get());

            if (input.peek() == ',' && input.good())
                input.ignore(1);

            // Parse the input
            std::vector<std::string> tokens = tokenize(reading.str(), "=");

            // Check if it is the shape
            if (tokens.size()==2 && strip(tokens[0])=="shape") {
                std::vector<std::string> dimensions = tokenize(tokens[1], " []");

                for (std::vector<std::string>::const_iterator it=dimensions.begin(); it!=dimensions.end(); ++it)
                    new_shape.push_back(from_string<Index>(*it));
            }

        } while(input.peek() != ')' && input.good());

        // Check that we reached the end of reading
        if (!input.good())
            throw TArrayReadErrorException("Reached end of input, but did not find the closing parenthesis.");

        // Skip the clsing parenthesis
        input.ignore();

        // Check that the shape was read correctly
        if (new_shape.size()==0)
            throw TArrayReadErrorException("The was not read.");

        Index new_asize=1;
        for (std::vector<Index>::const_iterator it=new_shape.begin(); it!=new_shape.end(); ++it)
            new_asize *= (*it);

        if (data.size()!=new_asize)
            throw TArrayReadErrorException("The specified shape does not match the number of data points.");

        reset_shape(new_shape);
    }
    else {
        reset_shape(newvector<Index>(data.size()));
    }

    // Put the data into the array
    for(typename TArray<T>::flatiterator it = get_flatiterator(); it(); ++it)
        *it = data[it.get_index()];

    return input;
}

/// Function for reading an array from a string representation.
///
/// \param input A string representation of an array.
/// \param full_format Whether the string representation is in the full format (including the shape of the array).
template<typename T>
inline void TArray<T>::read(const std::string &input, bool full_format) {
    std::istringstream tmp(input);
    read(tmp, full_format);
}


/// Output stream operator for the TArray.
template<typename T>
std::ostream &operator<<(std::ostream &output, const TArray<T> &array) {
    array.write(output);
    return output;
}

/// Input stream operator for the TArray.
template<typename T>
std::istream &operator>>(std::istream &input, TArray<T> &array) {
    array.read(input);
    return input;
}

/// A private getter that is used in both the constant and non-constant
/// overloading of the operator().
///
/// \param index The index of the elements.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::get(Index index) const {
    assert(index<asize);
    return array[index];
}

/// A private getter that is used in both the constant and non-constant
/// overloading of the operator().
///
/// \param coord1 The coordinate (index) in the first dimension.
/// \param coord2 The coordinate (index) in the second dimension.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::get(Index coord1, Index coord2) const {
    assert(ndims >= 2);
    Index index = coord1 + coord2 * stride[1];
    assert(index<asize);
    return array[index];
}

/// A private getter that is used in both the constant and non-constant
/// overloading of the operator().
///
/// \param coord The coordinate of the element.
/// \return The value with the given coordinate.
template<typename T>
inline T& TArray<T>::get(const std::vector<Index> &coord) const {
    assert(ndims >= coord.size());
    assert(coord.size()>0);
    Index index = 0;
    for (Dimension dim=0; dim<coord.size(); dim++) {
        index += coord[dim]*stride[dim];
    }
    assert(index<asize);
    return array[index];
}

} // namespace Muninn

#endif // MUNINN_TARRAY_H_

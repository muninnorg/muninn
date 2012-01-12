// BaseConverter.h
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

#ifndef MUNINN_BASECONVERTER_H_
#define MUNINN_BASECONVERTER_H_

#include <exception>
#include <string>

namespace Muninn {

/// Class with functions for casting (dynamic) a possible constant pointer or
/// reference from a base class to a derived class. The functions will throw an
/// exception (a specialization of std::bad_cast and MuninnException) that can
/// contains a message describing that the cast failed.
///
/// This class is mainly used for throwing an uncaught exception, if the
/// user combines classes that does not work together.
///
/// \tparam BASE The type of the base class.
/// \tparam DERIVED The type the base class is tried casted to.
template<class BASE, class DERIVED>
class BaseConverter {
public:

    /// This type of exception will be thrown if the cast fails.
    class CastException : public std::bad_cast, public MuninnException {
    public:

        /// Constructor of the exception.
        ///
        /// \param message The error message associated with the exception.
        CastException(const std::string &message) throw() : message(message) {}

        /// Default destructor.
        virtual ~CastException() throw() {};

        /// This function returns the error message and implements the
        /// std::bad_cast interface.
        ///
        /// \return The error message.
        virtual const char* what() const throw() {
            return message.c_str();
        }

    private:
        std::string message;
    };

    /// Cast a reference to an object of the base class to a reference of the
    /// derived class.
    ///
    /// \param base A reference to the base class.
    /// \param message The error message of the CastException that will be
    ///                thrown if the cast fails.
    /// \return A reference to the derived class.
    inline static DERIVED& cast_from_base(BASE& base, const std::string &message="") {
        DERIVED* derived = dynamic_cast<DERIVED*>(&base);
        if (derived==NULL)
            throw CastException(message);
        return *derived;
    }

    /// Cast a constant reference to an object of the base class to a reference
    /// of the derived class.
    ///
    /// \param base A constant reference to the base class.
    /// \param message The error message of the CastException that will be
    ///                thrown if the cast fails.
    /// \return A constant reference to the derived class.
    inline static const DERIVED& cast_from_base(const BASE& base, const std::string &message="") {
        const DERIVED* derived = dynamic_cast<const DERIVED*>(&base);
        if (derived==NULL)
            throw CastException(message);
        return *derived;
    }

    /// Cast a pointer to an object of the base class to a pointer of the
    /// derived class.
    ///
    /// \param base A pointer to the base class.
    /// \param message The error message of the CastException that will be
    ///                thrown if the cast fails.
    /// \return A pointer to the derived class.
    inline static DERIVED* cast_from_base(BASE* base, const std::string &message="") {
        DERIVED* derived = dynamic_cast<DERIVED*>(base);
        if (derived==NULL)
            throw CastException(message);
        return derived;
    }

    /// Cast a constant pointer to an object of the base class to a pointer
    /// of the derived class.
    ///
    /// \param base A constant pointer to the base class.
    /// \param message The error message of the CastException that will be
    ///                thrown if the cast fails.
    /// \return A constant pointer to the derived class.
    inline static const DERIVED* cast_from_base(const BASE* base, const std::string &message="") {
        const DERIVED* derived = dynamic_cast<const DERIVED*>(base);
        if (derived==NULL)
            throw CastException(message);
        return derived;
    }
};

} //namespace Muninn {

#endif // MUNINN_BASECONVERTER_H_

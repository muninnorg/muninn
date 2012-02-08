// MessageException.h
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

#ifndef MUNINN_MESSAGE_EXCEPTION_H_
#define MUNINN_MESSAGE_EXCEPTION_H_

#include <exception>
#include <string>

#include "muninn/Exceptions/MuninnException.h"

namespace Muninn {

/// A Muninn exception containing just a message. This is normally used for
/// reporting simple errors.
class MessageException : public MuninnException {
public:
    /// Constructor
    ///
    /// \param message The message to be returned by what().
    MessageException(const std::string &message) throw() : message(message) {}

    /// Virtual destructor
    virtual ~MessageException() throw() {};

    /// Returns a character string describing the cause of the error.
    ///
    /// \return A character string describing the cause of the error.
    virtual const char* what() const throw() {
        return message.c_str();
    }

private:
    std::string message;  ///< The message to be returned by what().
};

} // namespace Muninn

#endif /* MUNINN_MESSAGE_EXCEPTION_H_ */

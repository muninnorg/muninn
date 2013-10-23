// GenericEnumStreamOperators.h
// Copyright (c) 2013 Jes Frellsen
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

#ifndef MUNINN_GENERIC_ENUM_STREAM_OPERATORS_H_
#define MUNINN_GENERIC_ENUM_STREAM_OPERATORS_H_

#include <iostream>

namespace Muninn {

// Input GeEnum from string
template <class EnumType> std::istream &input_operator(std::istream &input, EnumType &ENUM_VALUE, const EnumType &ENUM_SIZE, const std::string EnumNames[]) {
     std::string raw_string;
     input >> raw_string;

     for (int i=0; i<ENUM_SIZE; ++i) {
          if (raw_string == EnumNames[i]) {
               ENUM_VALUE = EnumType(i);
          }
     }
     return input;
}

// Output GeEnum
template <class EnumType> std::ostream &output_operator(std::ostream &output, const EnumType &ENUM_VALUE, const EnumType &ENUM_SIZE, const std::string EnumNames[]) {
    assert(0<=ENUM_VALUE && ENUM_VALUE<ENUM_SIZE);
    output << EnumNames[static_cast<unsigned int>(ENUM_VALUE)];
    return output;
}

} //namespace Muninn {

#endif // MUNINN_GENERIC_ENUM_STREAM_OPERATORS_H_


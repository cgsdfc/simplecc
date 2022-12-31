// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/******************************************************************************
 *
 *  file:  ArgTraits.h
 *
 *  Copyright (c) 2007, Daniel Aarno, Michael E. Smoot .
 *  All rights reserved.
 *
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

// This is an internal tclap file, you should probably not have to
// include this directly

#ifndef TCLAP_ARGTRAITS_H
#define TCLAP_ARGTRAITS_H

namespace TCLAP {

// We use two empty structs to get compile type specialization
// function to work

/**
 * A value like argument value type is a value that can be set using
 * operator>>. This is the default value type.
 */
struct ValueLike {
  typedef ValueLike ValueCategory;
  virtual ~ValueLike() {}
};

/**
 * A string like argument value type is a value that can be set using
 * operator=(string). Useful if the value type contains spaces which
 * will be broken up into individual tokens by operator>>.
 */
struct StringLike {
  virtual ~StringLike() {}
};

/**
 * A class can inherit from this object to make it have string like
 * traits. This is a compile time thing and does not add any overhead
 * to the inherenting class.
 */
struct StringLikeTrait {
  typedef StringLike ValueCategory;
  virtual ~StringLikeTrait() {}
};

/**
 * A class can inherit from this object to make it have value like
 * traits. This is a compile time thing and does not add any overhead
 * to the inherenting class.
 */
struct ValueLikeTrait {
  typedef ValueLike ValueCategory;
  virtual ~ValueLikeTrait() {}
};

/**
 * Arg traits are used to get compile type specialization when parsing
 * argument values. Using an ArgTraits you can specify the way that
 * values gets assigned to any particular type during parsing. The two
 * supported types are StringLike and ValueLike.
 */
template <typename T> struct ArgTraits {
  typedef typename T::ValueCategory ValueCategory;
  virtual ~ArgTraits() {}
  // typedef ValueLike ValueCategory;
};

} // namespace TCLAP

#endif
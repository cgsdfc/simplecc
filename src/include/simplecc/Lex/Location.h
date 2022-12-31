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

#ifndef SIMPLECC_LEX_LOCATION_H
#define SIMPLECC_LEX_LOCATION_H
#include "simplecc/Support/Macros.h"
#include <iostream>

namespace simplecc {
/// This class represents a location in the source file.
class Location {
  unsigned Line;
  unsigned Column;

public:
  /// Construct a location at the beginning of the file.
  Location() : Line(0), Column(0) {}
  Location(unsigned lineno, unsigned col_offset)
      : Line(lineno), Column(col_offset) {}

  /// Return the line number.
  unsigned getLine() const { return Line; }

  /// Return the column offset.
  unsigned getColumn() const { return Column; }

  void Format(std::ostream &O) const;

  void FormatCompact(std::ostream &O) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(Location)
} // namespace simplecc

#endif // SIMPLECC_LEX_LOCATION_H
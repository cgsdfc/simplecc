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

#include "simplecc/Lex/TokenInfo.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace simplecc;

const char *TokenInfo::getSymbolName(Symbol S) {
  auto Val = static_cast<int>(S);
  return IsTerminal(S) ? TokenNames[Val] : SymbolNames[Val - NT_OFFSET];
}

void TokenInfo::Format(std::ostream &O) const {
  std::ostringstream OS;
  OS << Loc.getLine() << ',' << Loc.getColumn();
  O << std::left << std::setw(20) << OS.str();
  O << std::left << std::setw(15) << getTypeName();
  O << std::left << std::setw(15) << getString();
}

void Location::Format(std::ostream &O) const {
  O << "Location(" << Line << ", " << Column << ")";
}

void Location::FormatCompact(std::ostream &O) const {
  O << Line << ":" << Column << ":";
}

const char *TokenInfo::getTypeName() const {
  return getSymbolName(Type);
}

TokenInfo::TokenInfo(Symbol Ty, std::string S, Location Loc, std::string Line)
    : Type(Ty), Str(std::move(S)), Loc(Loc), Line(std::move(Line)) {
  assert(IsTerminal(Ty));
}

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

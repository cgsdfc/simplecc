#include "simplecc/Lex/TokenInfo.h"

using namespace simplecc;

const char *TokenInfo::getSymbolName(Symbol S) {
  auto Val = static_cast<int>(S);
  return IsTerminal(S) ? TokenNames[Val] : SymbolNames[Val - NT_OFFSET];
}

bool TokenInfo::IsTerminal(Symbol S) {
  return static_cast<int>(S) < NT_OFFSET;
}

void TokenInfo::Format(std::ostream &O) const {
  O << "TokenInfo("
    << "type=" << getTypeName() << ", "
    << "string=" << getString() << ", " << getLocation() << ", "
    << "line=" << getLine() << ")";
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
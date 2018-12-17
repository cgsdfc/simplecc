#ifndef TYPE_CHECKER
#define TYPE_CHECKER

namespace simplecc {
class Program;
class SymbolTable;

bool CheckType(Program *P, SymbolTable &S);
} // namespace simplecc
#endif

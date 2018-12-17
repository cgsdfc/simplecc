#ifndef TYPE_CHECKER
#define TYPE_CHECKER

namespace simplecompiler {
class Program;
class SymbolTable;

bool CheckType(Program *P, SymbolTable &S);
} // namespace simplecompiler
#endif

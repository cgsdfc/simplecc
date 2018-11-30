#ifndef TYPE_CHECK
#define TYPE_CHECK
#include "symtable.h"

namespace simplecompiler {
class Program;
bool CheckType(Program *prog, SymbolTable &symtable);
} // namespace simplecompiler
#endif

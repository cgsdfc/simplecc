#ifndef TYPE_CHECK
#define TYPE_CHECK
#include "SymbolTable.h"

namespace simplecompiler {
class Program;
bool CheckType(Program *prog, SymbolTable &symtable);
} // namespace simplecompiler
#endif

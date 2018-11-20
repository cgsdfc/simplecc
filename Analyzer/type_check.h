#ifndef TYPE_CHECK
#define TYPE_CHECK
#include "symtable.h"

class Program;
bool CheckType(Program *prog, SymbolTable &symtable);
#endif

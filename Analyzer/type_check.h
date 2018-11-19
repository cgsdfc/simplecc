#ifndef TYPE_CHECK
#define TYPE_CHECK
#include "AST.h"
#include "symtable.h"

bool CheckType(Program *prog, SymbolTable &symtable);
#endif

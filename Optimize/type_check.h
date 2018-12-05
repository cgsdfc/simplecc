#ifndef TYPE_CHECK
#define TYPE_CHECK

namespace simplecompiler {
class Program;
class SymbolTable;

bool CheckType(Program *prog, SymbolTable &symtable);
} // namespace simplecompiler
#endif

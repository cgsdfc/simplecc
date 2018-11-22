#ifndef COMPILE_H
#define COMPILE_H
#include "Opcode.h"
#include "symtable.h"

#include <vector>
#include <string>

using String = std::string;

class NameInfo {
public:
  String name;
  int size;
  NameInfo(const String &name, int size): name(name), size(size) {}
};

class Code {
public:
  // All local names: Variable and Array
  std::vector<NameInfo> names;
  // All byte code
  std::vector<unsigned char> code;
};

class Module {
public:
  // All int/char constants
  std::vector<int> consts;
  // All string literals
  std::vector<String> strings;
  // All global names: Variable, Function and Array
  std::vector<NameInfo> names;
  // All functions
  std::vector<Code*> code;
};

Module *Compile(Program *prog, const SymbolTable &symtable);
#endif

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

class ByteCode {
  Opcode opcode;
  int int_arg;
  const char *str_arg;
public:
  ByteCode(Opcode opcode): opcode(opcode) { }

  ByteCode(Opcode opcode, int arg): opcode(opcode), int_arg(arg) { }

  ByteCode(Opcode opcode, const char *arg): opcode(opcode), str_arg(arg) { }

  ByteCode(Opcode opcode, int int_arg, const char *str_arg):
    opcode(opcode), int_arg(int_arg), str_arg(str_arg) { }

  void SetTarget(int target) {
    int_arg = target;
  }

  Opcode GetOpcode() const { return opcode; }
  int GetIntOperand() const { return int_arg; }
  const char *GetStrOperand() const { return str_arg; }
};

class CompiledFunction {
public:
  // All local names: Variable and Array
  std::vector<NameInfo> names;
  // All byte code
  std::vector<ByteCode> code;
};

class CompiledModule {
public:
  // All int/char constants
  std::vector<int> consts;
  // All string literals
  std::vector<String> strings;
  // All global names: Variable, Function and Array
  std::vector<NameInfo> names;
  // All functions
  std::vector<CompiledFunction*> code;
};

/* Module *Compile(Program *prog, const SymbolTable &symtable); */
#endif

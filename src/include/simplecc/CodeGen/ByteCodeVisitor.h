#ifndef SIMPLECC_CODEGEN_BYTECODEVISITOR_H
#define SIMPLECC_CODEGEN_BYTECODEVISITOR_H
#include "simplecc/CodeGen/ByteCode.h"

namespace simplecc {
/// This class is a CRTP Mixin that provides a visit() method
/// to dispatch handling of each opcode to subclass.
/// Note: subclass must implement a corresponding visit method for **every**
/// opcode.
template <class Derived> class ByteCodeVisitor {
public:
  void visit(const ByteCode &C) {
    switch (C.getOpcode()) {
    default:
      assert(false && "Invalid Opcode");
#define HANDLE_OPCODE(opcode, camelName)                                       \
  case ByteCode::opcode:                                                       \
    return static_cast<Derived *>(this)->visit##camelName(C);
#include "simplecc/CodeGen/Opcode.def"
    }
  }
};

} // namespace simplecc
#endif
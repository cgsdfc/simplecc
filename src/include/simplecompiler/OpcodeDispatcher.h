
#ifndef OPCODE_DISPATCHER
#define OPCODE_DISPATCHER
#include "simplecompiler/ByteCode.h"

namespace simplecompiler {
template <class Derived> class OpcodeDispatcher {
public:
  void dispatch(const ByteCode &byteCode) {
    switch (byteCode.GetOpcode()) {

    case Opcode::LOAD_LOCAL:
      return static_cast<Derived *>(this)->HandleLoadLocal(byteCode);

    case Opcode::LOAD_GLOBAL:
      return static_cast<Derived *>(this)->HandleLoadGlobal(byteCode);

    case Opcode::LOAD_CONST:
      return static_cast<Derived *>(this)->HandleLoadConst(byteCode);

    case Opcode::LOAD_STRING:
      return static_cast<Derived *>(this)->HandleLoadString(byteCode);

    case Opcode::STORE_LOCAL:
      return static_cast<Derived *>(this)->HandleStoreLocal(byteCode);

    case Opcode::STORE_GLOBAL:
      return static_cast<Derived *>(this)->HandleStoreGlobal(byteCode);

    case Opcode::BINARY_ADD:
      return static_cast<Derived *>(this)->HandleBinaryAdd(byteCode);

    case Opcode::BINARY_SUB:
      return static_cast<Derived *>(this)->HandleBinarySub(byteCode);

    case Opcode::BINARY_MULTIPLY:
      return static_cast<Derived *>(this)->HandleBinaryMultiply(byteCode);

    case Opcode::BINARY_DIVIDE:
      return static_cast<Derived *>(this)->HandleBinaryDivide(byteCode);

    case Opcode::UNARY_POSITIVE:
      return static_cast<Derived *>(this)->HandleUnaryPositive(byteCode);

    case Opcode::UNARY_NEGATIVE:
      return static_cast<Derived *>(this)->HandleUnaryNegative(byteCode);

    case Opcode::CALL_FUNCTION:
      return static_cast<Derived *>(this)->HandleCallFunction(byteCode);

    case Opcode::RETURN_VALUE:
      return static_cast<Derived *>(this)->HandleReturnValue(byteCode);

    case Opcode::RETURN_NONE:
      return static_cast<Derived *>(this)->HandleReturnNone(byteCode);

    case Opcode::PRINT_STRING:
      return static_cast<Derived *>(this)->HandlePrintString(byteCode);

    case Opcode::PRINT_CHARACTER:
      return static_cast<Derived *>(this)->HandlePrintCharacter(byteCode);

    case Opcode::PRINT_INTEGER:
      return static_cast<Derived *>(this)->HandlePrintInteger(byteCode);

    case Opcode::PRINT_NEWLINE:
      return static_cast<Derived *>(this)->HandlePrintNewline(byteCode);

    case Opcode::READ_INTEGER:
      return static_cast<Derived *>(this)->HandleReadInteger(byteCode);

    case Opcode::READ_CHARACTER:
      return static_cast<Derived *>(this)->HandleReadCharacter(byteCode);

    case Opcode::BINARY_SUBSCR:
      return static_cast<Derived *>(this)->HandleBinarySubscr(byteCode);

    case Opcode::STORE_SUBSCR:
      return static_cast<Derived *>(this)->HandleStoreSubscr(byteCode);

    case Opcode::JUMP_IF_TRUE:
      return static_cast<Derived *>(this)->HandleJumpIfTrue(byteCode);

    case Opcode::JUMP_IF_FALSE:
      return static_cast<Derived *>(this)->HandleJumpIfFalse(byteCode);

    case Opcode::JUMP_FORWARD:
      return static_cast<Derived *>(this)->HandleJumpForward(byteCode);

    case Opcode::JUMP_IF_NOT_EQUAL:
      return static_cast<Derived *>(this)->HandleJumpIfNotEqual(byteCode);

    case Opcode::JUMP_IF_EQUAL:
      return static_cast<Derived *>(this)->HandleJumpIfEqual(byteCode);

    case Opcode::JUMP_IF_GREATER:
      return static_cast<Derived *>(this)->HandleJumpIfGreater(byteCode);

    case Opcode::JUMP_IF_GREATER_EQUAL:
      return static_cast<Derived *>(this)->HandleJumpIfGreaterEqual(byteCode);

    case Opcode::JUMP_IF_LESS:
      return static_cast<Derived *>(this)->HandleJumpIfLess(byteCode);

    case Opcode::JUMP_IF_LESS_EQUAL:
      return static_cast<Derived *>(this)->HandleJumpIfLessEqual(byteCode);

    case Opcode::POP_TOP:
      return static_cast<Derived *>(this)->HandlePopTop(byteCode);
    }
  }
};
} // namespace simplecompiler
#endif
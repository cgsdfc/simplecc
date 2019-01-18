#ifndef SIMPLECC_CODEGEN_BYTECODEBUILDER_H
#define SIMPLECC_CODEGEN_BYTECODEBUILDER_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/CodeGen/ByteCode.h"
#include "simplecc/CodeGen/ByteCodeFunction.h"
#include "simplecc/Parse/AST.h"
#include <cassert>
#include <utility> // forward()

namespace simplecc {
/// @brief ByteCodeBuilder provides helper methods to create ByteCode instance
/// and insert it at the end of a ByteCodeFunction, inspired by ``llvm::IRBuilder``.
/// ByteCodeBuilder aids ByteCodeCompiler in the process of walking the AST and generating a ByteCodeModule.
/// Essentially it does these things:
/// 1. manages the insert point of the ByteCodeFunction being generated.
/// 2. provides helpers to convert constructs of AST such as BinaryOpKind into ByteCode.
/// 3. manages to set the jump target correctly.
class ByteCodeBuilder {
  /// Forward Opcode from ByteCode.
  using Opcode = ByteCode::Opcode;

  /// Helpers to convert enum from AST to Opcode.
  static Opcode MakeSubScr(ExprContextKind ctx);
  static Opcode MakeLoad(Scope scope);
  static Opcode MakeStore(Scope scope);
  static Opcode MakeRead(BasicTypeKind type);
  static Opcode MakePrint(BasicTypeKind type);
  static Opcode MakeBinary(BinaryOpKind Op);
  static Opcode MakeUnary(UnaryOpKind Op);
  static Opcode MakeJumpNegative(BinaryOpKind Op);
  static Opcode MakeJump(BinaryOpKind Op);

  /// Insert a ByteCode into the back of InsertPoint.
  /// Return the offset of the inserted ByteCode.
  unsigned Insert(ByteCode Code);

  /// Create and insert a ByteCode.
  /// Return the offset of the inserted ByteCode.
  template <typename... Args>
  unsigned Create(Opcode Op, Args &&... ExtraArgs);

public:
  ByteCodeBuilder() = default;
  ~ByteCodeBuilder() = default;

  /// Create a read such as READ_INTEGER.
  unsigned CreateRead(BasicTypeKind T) { return Create(MakeRead(T)); }

  /// Create a print such as PRINT_INTEGER.
  unsigned CreatePrint(BasicTypeKind T) { return Create(MakePrint(T)); }

  /// Create a PRINT_NEWLINE.
  unsigned CreatePrintNewline() { return Create(ByteCode::PRINT_NEWLINE); }

  /// Create a PRINT_STRING.
  unsigned CreatePrintString() { return Create(ByteCode::PRINT_STRING); }

  /// Create a JUMP_IF_FALSE.
  unsigned CreateJumpIfFalse() { return Create(ByteCode::JUMP_IF_FALSE); }

  /// Create a JUMP_IF_TRUE.
  unsigned CreateJumpIfTrue() { return Create(ByteCode::JUMP_IF_TRUE); }

  /// Create a conditional jump, such as JUMP_IF_GREATER.
  /// If IsNeg is true, the jump conditions on the negative result of CompareOp.
  unsigned CreateCondJump(BinaryOpKind CompareOp, bool IsNeg = true) {
    return Create(IsNeg ? MakeJumpNegative(CompareOp) : MakeJump(CompareOp));
  }

  /// Create an unconditional jump (JUMP_FORWARD).
  unsigned CreateJumpForward() { return Create(ByteCode::JUMP_FORWARD); }

  /// Create a binary operation, such as BINARY_ADD.
  unsigned CreateBinary(BinaryOpKind Op) { return Create(MakeBinary(Op)); }

  /// Create a RETURN_VALUE.
  unsigned CreateReturnValue() { return Create(ByteCode::RETURN_VALUE); }

  /// Create a RETURN_NONE.
  unsigned CreateReturnNone() { return Create(ByteCode::RETURN_NONE); }

  /// Create a POP_TOP.
  unsigned CreatePopTop() { return Create(ByteCode::POP_TOP); }

  /// Create a unary operation, such as UNARY_NEGATIVE.
  unsigned CreateUnary(UnaryOpKind Op) { return Create(MakeUnary(Op)); }

  /// Create a CALL_FUNCTION.
  unsigned CreateCallFunction(const std::string &Name, unsigned Argc) {
    return Create(ByteCode::CALL_FUNCTION, Name.data(), Argc);
  }

  /// Create a load, such as LOAD_LOCAL.
  unsigned CreateLoad(Scope S, const std::string &Name) {
    return Create(MakeLoad(S), Name.data());
  }

  /// Create a store, such as STORE_LOCAL.
  unsigned CreateStore(Scope S, const std::string &Name) {
    return Create(MakeStore(S), Name.data());
  }

  /// Create a LOAD_CONST.
  unsigned CreateLoadConst(int ConstValue) {
    return Create(ByteCode::LOAD_CONST, ConstValue);
  }

  /// Create a LOAD_STRING.
  unsigned CreateLoadString(unsigned StringID) {
    return Create(ByteCode::LOAD_STRING, StringID);
  }

  /// Create a subscript operation, such as BINARY_SUBSCR and STORE_SUBSCR.
  unsigned CreateSubscr(ExprContextKind Context) {
    return Create(MakeSubScr(Context));
  }

  /// Set the jump target of ByteCode at offset Idx.
  /// Require that ByteCode has a jump Opcode.
  /// Commonly used with CreateCondJump() or CreateJumpForward().
  /// Example: @code
  /// auto Jump = Builder.CreateJumpForward();
  /// auto Off = Builder.getSize();
  /// Builder.setJumpTargetAt(Jump, Off);
  /// @endcode
  void setJumpTargetAt(unsigned Idx, unsigned Target);

  /// Set the insert point to F.
  /// Subsequent call to Create() family will insert ByteCode at the end of F.
  void setInsertPoint(ByteCodeFunction *F) { InsertPoint = F; }

  /// Return the insert point.
  ByteCodeFunction *getInsertPoint() const {
    assert(InsertPoint && "InsertPoint not set!");
    return InsertPoint;
  }

  /// Set the current lineno.
  void setLineno(Location L) { CurrentLineno = L.getLine(); }
  /// Return the current lineno.
  unsigned getLineno() const { return CurrentLineno; }

  /// Return the size of the InstList of the insert point.
  unsigned int getSize() const { return getInsertPoint()->size(); }
private:
  ByteCodeFunction *InsertPoint = nullptr;
  unsigned CurrentLineno = 1;
};

template <typename... Args>
unsigned ByteCodeBuilder::Create(Opcode Op, Args &&... ExtraArgs) {
  return Insert(ByteCode::Create(Op, std::forward<Args>(ExtraArgs)...));
}
} // namespace simplecc
#endif // SIMPLECC_CODEGEN_BYTECODEBUILDER_H
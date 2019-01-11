#ifndef SIMPLECC_CODEGEN_BYTECODEBUILDER_H
#define SIMPLECC_CODEGEN_BYTECODEBUILDER_H
#include "simplecc/Analysis/Types.h"
#include "simplecc/CodeGen/ByteCode.h"
#include "simplecc/Parse/AST.h"
#include <cassert>

namespace simplecc {
class ByteCodeFunction;

class ByteCodeBuilder {
  static unsigned MakeSubScr(ExprContextKind ctx);
  static unsigned MakeLoad(Scope scope);
  static unsigned MakeStore(Scope scope);
  static unsigned MakeRead(BasicTypeKind type);
  static unsigned MakePrint(BasicTypeKind type);
  static unsigned MakeBinary(BinaryOpKind Op);
  static unsigned MakeUnary(UnaryOpKind Op);
  static unsigned MakeJumpNegative(BinaryOpKind Op);
  static unsigned MakeJump(BinaryOpKind Op);

  /// Insert a ByteCode into the back of InsertPoint.
  /// Return the offset of the inserted ByteCode.
  //
  unsigned Insert(ByteCode Code);

  /// Create a ByteCode and insert it into the InsertPoint.
  /// This essentially forwards arguments to ByteCode::Create().
  //
  template <typename... Args>
  unsigned Create(unsigned Op, Args &&... ExtraArgs) {
    return Insert(ByteCode::Create(static_cast<ByteCode::Opcode>(Op),
                                   std::forward<Args>(ExtraArgs)...));
  }
public:
  ByteCodeBuilder() = default;
  ~ByteCodeBuilder() = default;

  /// ByteCode creation helpers:
  unsigned CreateRead(BasicTypeKind T) { return Create(MakeRead(T)); }
  unsigned CreatePrint(BasicTypeKind T) { return Create(MakePrint(T)); }
  unsigned CreatePrintNewline() { return Create(ByteCode::PRINT_NEWLINE); }
  unsigned CreatePrintString() { return Create(ByteCode::PRINT_STRING); }
  unsigned CreateJumpIfFalse() { return Create(ByteCode::JUMP_IF_FALSE); }
  unsigned CreateJumpIfTrue() { return Create(ByteCode::JUMP_IF_TRUE); }

  unsigned CreateJump(BinaryOpKind CompareOp, bool IsNeg = true) {
    return Create(IsNeg ? MakeJumpNegative(CompareOp) : MakeJump(CompareOp));
  }

  unsigned CreateJumpForward() { return Create(ByteCode::JUMP_FORWARD); }
  unsigned CreateBinary(BinaryOpKind Op) { return Create(MakeBinary(Op)); }
  unsigned CreateReturnValue() { return Create(ByteCode::RETURN_VALUE); }
  unsigned CreateReturnNone() { return Create(ByteCode::RETURN_NONE); }
  unsigned CreatePopTop() { return Create(ByteCode::POP_TOP); }
  unsigned CreateUnary(UnaryOpKind Op) { return Create(MakeUnary(Op)); }

  unsigned CreateCallFunction(const std::string &Name, unsigned Argc) {
    return Create(ByteCode::CALL_FUNCTION, Name.data(), Argc);
  }
  unsigned CreateLoad(Scope S, const std::string &Name) {
    return Create(MakeLoad(S), Name.data());
  }
  unsigned CreateStore(Scope S, const std::string &Name) {
    return Create(MakeStore(S), Name.data());
  }
  unsigned CreateLoadConst(int ConstValue) {
    return Create(ByteCode::LOAD_CONST, ConstValue);
  }
  unsigned CreateLoadString(unsigned StringID) {
    return Create(ByteCode::LOAD_STRING, StringID);
  }
  unsigned CreateSubscr(ExprContextKind Context) {
    return Create(MakeSubScr(Context));
  }

  void setJumpTargetAt(unsigned Idx, unsigned Target);

  void setInsertPoint(ByteCodeFunction *F) { InsertPoint = F; }
  ByteCodeFunction *getInsertPoint() const {
    assert(InsertPoint && "InsertPoint not set!");
    return InsertPoint;
  }

  void setLocation(Location L) { CurrentLineno = L.getLineNo(); }
  unsigned getLineNo() const { return CurrentLineno; }

  /// Return the size of the current ByteCodeFunction.
  unsigned getSize() const;
private:
  ByteCodeFunction *InsertPoint = nullptr;
  unsigned CurrentLineno = 1;
};
} // namespace simplecc

#endif
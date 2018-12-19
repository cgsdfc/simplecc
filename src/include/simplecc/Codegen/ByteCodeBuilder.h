#ifndef BYTE_CODE_BUILDER_H
#define BYTE_CODE_BUILDER_H
#include "simplecc/Parse/AST.h"
#include "simplecc/Codegen/ByteCode.h"
#include "simplecc/Analysis/Types.h"

#include <cassert>

namespace simplecc {
class ByteCodeFunction;

class ByteCodeBuilder {

  static Opcode MakeSubScr(ExprContextKind ctx);

  static Opcode MakeLoad(Scope scope);

  static Opcode MakeStore(Scope scope);

  static Opcode MakeRead(BasicTypeKind type);

  static Opcode MakePrint(BasicTypeKind type);

  static Opcode MakeBinary(OperatorKind Op);

  static Opcode MakeUnary(UnaryopKind Op);

  static Opcode MakeJumpNegative(OperatorKind Op);

  static Opcode MakeJump(OperatorKind Op);

  /// Insert a ByteCode into the back of InsertPoint.
  /// Return the offset of the inserted ByteCode.
  //
  unsigned Insert(ByteCode Code);

  /// Create a ByteCode and insert it into the InsertPoint.
  /// This essentially forwards arguments to ByteCode::Create().
  //
  template <typename... Args> unsigned Create(Opcode Op, Args &&... ExtraArgs) {
    return Insert(ByteCode::Create(Op, std::forward<Args>(ExtraArgs)...));
  }

public:
  ByteCodeBuilder() = default;
  ~ByteCodeBuilder() = default;

  /// ByteCode creation helpers:
  unsigned CreateRead(BasicTypeKind T) { return Create(MakeRead(T)); }
  unsigned CreatePrint(BasicTypeKind T) { return Create(MakePrint(T)); }
  unsigned CreatePrintNewline() { return Create(Opcode::PRINT_NEWLINE); }
  unsigned CreatePrintString() { return Create(Opcode::PRINT_STRING); }
  unsigned CreateJumpIfFalse() { return Create(Opcode::JUMP_IF_FALSE); }
  unsigned CreateJumpIfTrue() { return Create(Opcode::JUMP_IF_TRUE); }

  unsigned CreateJump(OperatorKind CompareOp, bool IsNeg = true) {
    return Create(IsNeg ? MakeJumpNegative(CompareOp) : MakeJump(CompareOp));
  }

  unsigned CreateJumpForward() { return Create(Opcode::JUMP_FORWARD); }
  unsigned CreateBinary(OperatorKind Op) { return Create(MakeBinary(Op)); }
  unsigned CreateReturnValue() { return Create(Opcode::RETURN_VALUE); }
  unsigned CreateReturnNone() { return Create(Opcode::RETURN_NONE); }
  unsigned CreatePopTop() { return Create(Opcode::POP_TOP); }
  unsigned CreateUnary(UnaryopKind Op) { return Create(MakeUnary(Op)); }

  unsigned CreateCallFunction(const String &Name, unsigned Argc) {
    return Create(Opcode::CALL_FUNCTION, Name.data(), Argc);
  }

  unsigned CreateLoad(Scope S, const String &Name) {
    return Create(MakeLoad(S), Name.data());
  }

  unsigned CreateStore(Scope S, const String &Name) {
    return Create(MakeStore(S), Name.data());
  }

  unsigned CreateLoadConst(int ConstValue) {
    return Create(Opcode::LOAD_CONST, ConstValue);
  }

  unsigned CreateLoadString(unsigned StringID) {
    return Create(Opcode::LOAD_STRING, StringID);
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

  void setLocation(const Location &L) { CurrentLineno = L.getLineNo(); }
  unsigned getLineNo() const { return CurrentLineno; }

  /// Return the size of the current ByteCodeFunction.
  unsigned getSize() const;

private:
  ByteCodeFunction *InsertPoint = nullptr;
  unsigned CurrentLineno = 1;
};
} // namespace simplecc

#endif

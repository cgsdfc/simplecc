#ifndef SIMPLECC_TARGET_BYTECODETOMIPSTRANSLATOR_H
#define SIMPLECC_TARGET_BYTECODETOMIPSTRANSLATOR_H
#include "simplecc/CodeGen/ByteCodeVisitor.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Target/MipsSupport.h"
#include <iostream>
#include <utility>

namespace simplecc {
class LocalContext;

// Serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator : ByteCodeVisitor<ByteCodeToMipsTranslator> {

  // Push a register onto the stack
  void PUSH(const char *R);

  // Pop the stack, optionally taking the tos value
  void POP(const char *R = nullptr);

  /// Load Operator
  void visitLoadLocal(const ByteCode &C);
  void visitLoadGlobal(const ByteCode &C);
  void visitLoadConst(const ByteCode &C);
  void visitLoadString(const ByteCode &C);

  /// Store Operator
  void visitStoreLocal(const ByteCode &C);
  void visitStoreGlobal(const ByteCode &C);

  /// Binary Operator
  void visitBinary(const char *Op);
  void visitBinaryAdd(const ByteCode &C);
  void visitBinarySub(const ByteCode &C);
  void visitBinaryMultiply(const ByteCode &C);
  void visitBinaryDivide(const ByteCode &C);

  /// Unary Operator.
  void visitUnaryPositive(const ByteCode &C) {}
  void visitUnaryNegative(const ByteCode &C);

  /// Call and Return.
  void visitCallFunction(const ByteCode &C);
  void visitReturn();
  void visitReturnValue(const ByteCode &C);
  void visitReturnNone(const ByteCode &C) { visitReturn(); }

  /// Print Operators
  void visitPrint(MipsSyscallCode Syscall);
  void visitPrintString(const ByteCode &C);
  void visitPrintCharacter(const ByteCode &C);
  void visitPrintInteger(const ByteCode &C);
  void visitPrintNewline(const ByteCode &C);

  /// Read Operators
  void visitRead(MipsSyscallCode Syscall);
  void visitReadInteger(const ByteCode &C);
  void visitReadCharacter(const ByteCode &C);

  /// Subscript Operators
  void visitBinarySubscr(const ByteCode &C);
  void visitStoreSubscr(const ByteCode &C);

  /// Jump Operators.
  void visitUnaryJumpIf(const char *op, const ByteCode &C);
  void visitJumpIfTrue(const ByteCode &C);
  void visitJumpIfFalse(const ByteCode &C);
  void visitJumpForward(const ByteCode &C);
  void visitBinaryJumpIf(const char *Op, const ByteCode &C);
  void visitJumpIfNotEqual(const ByteCode &C);
  void visitJumpIfEqual(const ByteCode &C);
  void visitJumpIfGreater(const ByteCode &C);
  void visitJumpIfGreaterEqual(const ByteCode &C);
  void visitJumpIfLess(const ByteCode &C);
  void visitJumpIfLessEqual(const ByteCode &C);

  void visitPopTop(const ByteCode &C) { POP(); }

  /// Forward WriteLine() to ThePrinter.
  template <typename... Args> void WriteLine(Args &&... Arguments) {
    ThePrinter.WriteLine(std::forward<Args>(Arguments)...);
  }

public:
  ByteCodeToMipsTranslator(std::ostream &O, const LocalContext &C)
      : ThePrinter(O), ByteCodeVisitor(), TheContext(C) {}

  /// Wrap OpcodeDispatcher::dispatch() to provide label
  /// generation.
  void Write(const ByteCode &C);

private:
  friend class ByteCodeVisitor<ByteCodeToMipsTranslator>;
  Printer ThePrinter;
  const LocalContext &TheContext;
  /// Keep track of the depth of stack.
  int StackDepth = 0;
};
} // namespace simplecc

#endif
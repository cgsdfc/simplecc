#ifndef BYTE_CODE_TO_MIPS_TRANSLATOR_H
#define BYTE_CODE_TO_MIPS_TRANSLATOR_H
#include "simplecc/MipsSupport.h"
#include "simplecc/OpcodeDispatcher.h"
#include "simplecc/Print.h"

#include <iostream>
#include <utility>

namespace simplecc {
class LocalContext;

// Serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator : OpcodeDispatcher<ByteCodeToMipsTranslator> {

  // Push a register onto the stack
  void PUSH(const char *R);

  // Pop the stack, optionally taking the tos value
  void POP(const char *R = nullptr);

  /// Load Operator
  void HandleLoadLocal(const ByteCode &C);
  void HandleLoadGlobal(const ByteCode &C);
  void HandleLoadConst(const ByteCode &C);
  void HandleLoadString(const ByteCode &C);

  /// Store Operator
  void HandleStoreLocal(const ByteCode &C);
  void HandleStoreGlobal(const ByteCode &C);

  /// Binary Operator
  void HandleBinary(const char *Op);
  void HandleBinaryAdd(const ByteCode &C);
  void HandleBinarySub(const ByteCode &C);
  void HandleBinaryMultiply(const ByteCode &C);
  void HandleBinaryDivide(const ByteCode &C);

  /// Unary Operator.
  void HandleUnaryPositive(const ByteCode &C) {}
  void HandleUnaryNegative(const ByteCode &C);

  /// Call and Return.
  void HandleCallFunction(const ByteCode &C);
  void HandleReturn();
  void HandleReturnValue(const ByteCode &C);
  void HandleReturnNone(const ByteCode &C) { HandleReturn(); }

  /// Print Operators
  void HandlePrint(MipsSyscallCode Syscall);
  void HandlePrintString(const ByteCode &C);
  void HandlePrintCharacter(const ByteCode &C);
  void HandlePrintInteger(const ByteCode &C);
  void HandlePrintNewline(const ByteCode &C);

  /// Read Operators
  void HandleRead(MipsSyscallCode Syscall);
  void HandleReadInteger(const ByteCode &C);
  void HandleReadCharacter(const ByteCode &C);

  /// Subscript Operators
  void HandleBinarySubscr(const ByteCode &C);
  void HandleStoreSubscr(const ByteCode &C);

  /// Jump Operators.
  void HandleUnaryJumpIf(const char *op, const ByteCode &C);
  void HandleJumpIfTrue(const ByteCode &C);
  void HandleJumpIfFalse(const ByteCode &C);
  void HandleJumpForward(const ByteCode &C);
  void HandleBinaryJumpIf(const char *Op, const ByteCode &C);
  void HandleJumpIfNotEqual(const ByteCode &C);
  void HandleJumpIfEqual(const ByteCode &C);
  void HandleJumpIfGreater(const ByteCode &C);
  void HandleJumpIfGreaterEqual(const ByteCode &C);
  void HandleJumpIfLess(const ByteCode &C);
  void HandleJumpIfLessEqual(const ByteCode &C);

  void HandlePopTop(const ByteCode &C) { POP(); }

  /// Forward WriteLine() to ThePrinter.
  template <typename... Args> void WriteLine(Args &&... Arguments) {
    ThePrinter.WriteLine(std::forward<Args>(Arguments)...);
  }

public:
  ByteCodeToMipsTranslator(std::ostream &O, const LocalContext &C)
      : ThePrinter(O), OpcodeDispatcher(), TheContext(C) {}

  /// Wrap OpcodeDispatcher::dispatch() to provide label
  /// generation.
  void Write(const ByteCode &C);

private:
  friend class OpcodeDispatcher<ByteCodeToMipsTranslator>;
  Printer ThePrinter;
  const LocalContext &TheContext;
  /// Keep track of the depth of stack.
  int StackDepth = 0;
};
} // namespace simplecc

#endif

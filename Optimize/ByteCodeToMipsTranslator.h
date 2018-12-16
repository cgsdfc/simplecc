#ifndef BYTE_CODE_TO_MIPS_TRANSLATOR_H
#define BYTE_CODE_TO_MIPS_TRANSLATOR_H
#include "MipsSupport.h"
#include "OpcodeDispatcher.h"
#include "Print.h"

#include <iostream>
#include <utility>

namespace simplecompiler {
class LocalContext;

// Serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator
    : OpcodeDispatcher<ByteCodeToMipsTranslator> {

  // Push a register onto the stack
  void PUSH(const char *r);

  // Pop the stack, optionally taking the tos value
  void POP(const char *r = nullptr);

  void HandleLoadLocal(const ByteCode &C);

  void HandleLoadGlobal(const ByteCode &C);

  void HandleLoadConst(const ByteCode &C);

  void HandleLoadString(const ByteCode &C);

  void HandleStoreLocal(const ByteCode &C);

  void HandleStoreGlobal(const ByteCode &C);

  void HandleBinary(const char *op);

  void HandleBinaryAdd(const ByteCode &C) { HandleBinary("add"); }

  void HandleBinarySub(const ByteCode &C) { HandleBinary("sub"); }

  void HandleBinaryMultiply(const ByteCode &C) { HandleBinary("mul"); }

  void HandleBinaryDivide(const ByteCode &C) { HandleBinary("div"); }

  /// Nop
  void HandleUnaryPositive(const ByteCode &C) {}

  void HandleUnaryNegative(const ByteCode &C);

  void HandleCallFunction(const ByteCode &C);

  void HandleReturn();

  void HandleReturnValue(const ByteCode &C);

  void HandleReturnNone(const ByteCode &C) { HandleReturn(); }

  void HandlePrint(MipsSyscallNumber syscall_code);

  void HandlePrintString(const ByteCode &C);

  void HandlePrintCharacter(const ByteCode &C);

  void HandlePrintInteger(const ByteCode &C);

  void HandlePrintNewline(const ByteCode &C);

  void HandleRead(MipsSyscallNumber syscall_code);

  void HandleReadInteger(const ByteCode &C);

  void HandleReadCharacter(const ByteCode &C);

  void HandleBinarySubscr(const ByteCode &C);

  void HandleStoreSubscr(const ByteCode &C);

  void HandleUnaryJumpIf(const char *op, const ByteCode &C);

  void HandleJumpIfTrue(const ByteCode &C) { HandleUnaryJumpIf("bnez", C); }

  void HandleJumpIfFalse(const ByteCode &C) { HandleUnaryJumpIf("beqz", C); }

  void HandleJumpForward(const ByteCode &C);

  void HandleBinaryJumpIf(const char *Op, const ByteCode &C);

  void HandleJumpIfNotEqual(const ByteCode &C) { HandleBinaryJumpIf("bne", C); }

  void HandleJumpIfEqual(const ByteCode &C) { HandleBinaryJumpIf("beq", C); }

  void HandleJumpIfGreater(const ByteCode &C) { HandleBinaryJumpIf("bgt", C); }

  void HandleJumpIfGreaterEqual(const ByteCode &C) {
    HandleBinaryJumpIf("bge", C);
  }

  void HandleJumpIfLess(const ByteCode &C) { HandleBinaryJumpIf("blt", C); }

  void HandleJumpIfLessEqual(const ByteCode &C) {
    HandleBinaryJumpIf("ble", C);
  }

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
  int stack_level = 0;
};
} // namespace simplecompiler

#endif

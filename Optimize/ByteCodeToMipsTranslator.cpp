#include "ByteCodeToMipsTranslator.h"
#include "LocalContext.h"
#include <cassert>

using namespace simplecompiler;

// Push a register onto the stack
void ByteCodeToMipsTranslator::PUSH(const char *r) {
  // $sp points to the one entry pass TOS
  assert(r);
  assert(r[0] == '$');
  WriteLine("sw", r, ", 0($sp)");
  WriteLine("addi $sp, $sp, -4");
  ++stack_level;
}

// Pop the stack, optionally taking the tos value
void ByteCodeToMipsTranslator::POP(const char *r) {
  assert(stack_level > 0 && "POP an empty stack!");
  WriteLine("addi $sp, $sp, 4");
  if (r) {
    assert(r[0] == '$');
    WriteLine("lw", r, ", 0($sp)");
  }
  --stack_level;
}

void ByteCodeToMipsTranslator::HandleBinarySubscr(const ByteCode &C) {
  // elements of smaller index are stored at higher address
  // as opposed to C convension.
  POP("$t0");                     // index
  POP("$t1");                     // base
  WriteLine("sll $t0, $t0, 2");   // offset = index * 4
  WriteLine("add $t2, $t1, $t0"); // address = base + offset
  WriteLine("lw $t3, 0($t2)");    // t3 = array[index]
  PUSH("$t3");
}

void ByteCodeToMipsTranslator::HandleLoadLocal(const ByteCode &C) {
  auto offset = TheContext.GetLocalOffset(C.GetStrOperand());
  if (TheContext.IsVariable(C.GetStrOperand())) {
    WriteLine("lw $t0,", offset, "($fp)");
  } else {
    WriteLine("addi $t0, $fp,", offset);
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadGlobal(const ByteCode &C) {
  GlobalLabel GL(C.GetStrOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", GL);
  if (TheContext.IsVariable(C.GetStrOperand())) {
    WriteLine("lw $t0, 0($t0)");
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadConst(const ByteCode &C) {
  WriteLine("li $t0,", C.GetIntOperand());
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadString(const ByteCode &C) {
  AsciizLabel AL(C.GetIntOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", AL);
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleStoreGlobal(const ByteCode &C) {
  POP("$t0");
  GlobalLabel GL(C.GetStrOperand(), /* NeedColon */ false);
  WriteLine("sw $t0,", GL);
}

void ByteCodeToMipsTranslator::HandleStoreLocal(const ByteCode &C) {
  POP("$t0");
  auto offset = TheContext.GetLocalOffset(C.GetStrOperand());
  WriteLine("sw $t0,", offset, "($fp)");
}

void ByteCodeToMipsTranslator::HandleBinary(const char *op) {
  POP("$t0"); // TOS
  POP("$t1"); // TOS1
  WriteLine(op, "$t2, $t1, $t0");
  PUSH("$t2");
}

void ByteCodeToMipsTranslator::HandleUnaryNegative(const ByteCode &C) {
  // $sp points to **the next vacant byte** of the stack, so
  // TOS is 4 + $sp
  WriteLine("lw $t0, 4($sp)");
  WriteLine("sub $t0, $zero, $t0");
  WriteLine("sw, $t0, 4($sp)");
}

void ByteCodeToMipsTranslator::HandleCallFunction(const ByteCode &C) {
  GlobalLabel Fn(C.GetStrOperand(), /* NeedColon */ false);
  WriteLine("jal", Fn);
  auto bytes = BytesFromEntries(C.GetIntOperand());
  if (bytes) {
    WriteLine("addi $sp, $sp", bytes);
  }
  PUSH("$v0");
}

void ByteCodeToMipsTranslator::HandleReturn() {
  WriteLine("j", ReturnLabel(TheContext.getName(), /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::HandleReturnValue(const ByteCode &C) {
  POP("$v0");
  HandleReturn();
}

void ByteCodeToMipsTranslator::HandlePrint(MipsSyscallNumber syscall_code) {
  WriteLine("li $v0,", syscall_code);
  POP("$a0");
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::HandlePrintString(const ByteCode &C) {
  /* print string */
  /* v0 = 4 */
  /* $a0 = address of null-terminated string to print */
  HandlePrint(MipsSyscallNumber::PRINT_STRING);
}

void ByteCodeToMipsTranslator::HandlePrintCharacter(const ByteCode &C) {
  /* print character */
  /* 11 */
  /* $a0 = character to print */
  HandlePrint(MipsSyscallNumber::PRINT_CHARACTER);
}

void ByteCodeToMipsTranslator::HandlePrintInteger(const ByteCode &C) {
  /* print integer */
  /* 1 */
  /* $a0 = integer to print */
  HandlePrint(MipsSyscallNumber::PRINT_INTEGER);
}

void ByteCodeToMipsTranslator::HandlePrintNewline(const ByteCode &C) {
  WriteLine("li $a0,", static_cast<int>('\n'));
  WriteLine("li $v0,", MipsSyscallNumber::PRINT_CHARACTER);
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::HandleRead(MipsSyscallNumber syscall_code) {
  WriteLine("li $v0,", syscall_code);
  WriteLine("syscall");
  PUSH("$v0");
}

void ByteCodeToMipsTranslator::HandleReadInteger(const ByteCode &C) {
  /* read integer */
  /* 5 */
  /* $v0 contains integer read */
  HandleRead(MipsSyscallNumber::READ_INTEGER);
}

void ByteCodeToMipsTranslator::HandleReadCharacter(const ByteCode &C) {
  /* read character */
  /* 12 */
  /* $v0 contains character read */
  HandleRead(MipsSyscallNumber::READ_CHARACTER);
}

void ByteCodeToMipsTranslator::HandleStoreSubscr(const ByteCode &C) {
  POP("$t0");
  POP("$t1");
  POP("$t3");
  WriteLine("sll $t0, $t0, 2");
  WriteLine("add $t2, $t1, $t0");
  WriteLine("sw $t3, 0($t2)");
}

void ByteCodeToMipsTranslator::HandleUnaryJumpIf(const char *op,
                                                 const ByteCode &C) {
  POP("$t0");
  WriteLine(op, "$t0",
            JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                            /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::HandleJumpForward(const ByteCode &C) {
  WriteLine("j", JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                                 /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::HandleBinaryJumpIf(const char *Op,
                                                  const ByteCode &C) {
  POP("$t0"); // TOS
  POP("$t1"); // TOS1
  WriteLine(Op, "$t1, $t0,",
            JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                            /* NeedColon */ false));
}

/// ByteCodeToMipsTranslator::Wrap OpcodeDispatcher::dispatch() to provide label
/// generation.
void ByteCodeToMipsTranslator::Write(const ByteCode &C) {
  unsigned Off = C.GetByteCodeOffset();
  /// If this is a JumpTarget, emit a label.
  if (TheContext.IsJumpTarget(Off)) {
    WriteLine(JumpTargetLabel(TheContext.getName(), Off, /* NeedColon */ true));
  }
  WriteLine("#", C.GetOpcodeName());
  OpcodeDispatcher::dispatch(C);
  WriteLine();
}

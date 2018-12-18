#include "simplecc/ByteCodeToMipsTranslator.h"
#include "simplecc/LocalContext.h"
#include <cassert>

using namespace simplecc;

// Push a register onto the stack
void ByteCodeToMipsTranslator::PUSH(const char *R) {
  // $sp points to the one entry pass TOS
  assert(R);
  assert(R[0] == '$');
  WriteLine("sw", R, ", 0($sp)");
  WriteLine("addi $sp, $sp, -4");
  ++StackDepth;
}

// Pop the stack, optionally taking the tos value
void ByteCodeToMipsTranslator::POP(const char *R) {
  assert(StackDepth > 0 && "POP an empty stack!");
  WriteLine("addi $sp, $sp, 4");
  if (R) {
    assert(R[0] == '$');
    WriteLine("lw", R, ", 0($sp)");
  }
  --StackDepth;
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
  auto offset = TheContext.GetLocalOffset(C.getStrOperand());
  if (TheContext.IsVariable(C.getStrOperand())) {
    WriteLine("lw $t0,", offset, "($fp)");
  } else {
    WriteLine("addi $t0, $fp,", offset);
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadGlobal(const ByteCode &C) {
  GlobalLabel GL(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", GL);
  if (TheContext.IsVariable(C.getStrOperand())) {
    WriteLine("lw $t0, 0($t0)");
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadConst(const ByteCode &C) {
  WriteLine("li $t0,", C.getIntOperand());
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleLoadString(const ByteCode &C) {
  AsciizLabel AL(C.getIntOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", AL);
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::HandleStoreGlobal(const ByteCode &C) {
  POP("$t0");
  GlobalLabel GL(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("sw $t0,", GL);
}

void ByteCodeToMipsTranslator::HandleStoreLocal(const ByteCode &C) {
  POP("$t0");
  auto offset = TheContext.GetLocalOffset(C.getStrOperand());
  WriteLine("sw $t0,", offset, "($fp)");
}

void ByteCodeToMipsTranslator::HandleBinary(const char *op) {
  POP("$t0"); // TOS
  POP("$t1"); // TOS1
  WriteLine(op, "$t2, $t1, $t0");
  PUSH("$t2");
}

void ByteCodeToMipsTranslator::HandleBinaryAdd(const ByteCode &C) {
  HandleBinary("add");
}
void ByteCodeToMipsTranslator::HandleBinarySub(const ByteCode &C) {
  HandleBinary("sub");
}
void ByteCodeToMipsTranslator::HandleBinaryMultiply(const ByteCode &C) {
  HandleBinary("mul");
}
void ByteCodeToMipsTranslator::HandleBinaryDivide(const ByteCode &C) {
  HandleBinary("div");
}

void ByteCodeToMipsTranslator::HandleUnaryNegative(const ByteCode &C) {
  // $sp points to **the next vacant byte** of the stack, so
  // TOS is 4 + $sp
  WriteLine("lw $t0, 4($sp)");
  WriteLine("sub $t0, $zero, $t0");
  WriteLine("sw, $t0, 4($sp)");
}

void ByteCodeToMipsTranslator::HandleCallFunction(const ByteCode &C) {
  GlobalLabel Fn(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("jal", Fn);
  auto bytes = BytesFromEntries(C.getIntOperand());
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

void ByteCodeToMipsTranslator::HandlePrint(MipsSyscallCode Syscall) {
  WriteLine("li $v0,", Syscall);
  POP("$a0");
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::HandlePrintString(const ByteCode &C) {
  /* print string */
  /* v0 = 4 */
  /* $a0 = address of null-terminated string to print */
  HandlePrint(MipsSyscallCode::PRINT_STRING);
}

void ByteCodeToMipsTranslator::HandlePrintCharacter(const ByteCode &C) {
  /* print character */
  /* 11 */
  /* $a0 = character to print */
  HandlePrint(MipsSyscallCode::PRINT_CHARACTER);
}

void ByteCodeToMipsTranslator::HandlePrintInteger(const ByteCode &C) {
  /* print integer */
  /* 1 */
  /* $a0 = integer to print */
  HandlePrint(MipsSyscallCode::PRINT_INTEGER);
}

void ByteCodeToMipsTranslator::HandlePrintNewline(const ByteCode &C) {
  WriteLine("li $a0,", static_cast<int>('\n'));
  WriteLine("li $v0,", MipsSyscallCode::PRINT_CHARACTER);
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::HandleRead(MipsSyscallCode Syscall) {
  WriteLine("li $v0,", Syscall);
  WriteLine("syscall");
  PUSH("$v0");
}

void ByteCodeToMipsTranslator::HandleReadInteger(const ByteCode &C) {
  /* read integer */
  /* 5 */
  /* $v0 contains integer read */
  HandleRead(MipsSyscallCode::READ_INTEGER);
}

void ByteCodeToMipsTranslator::HandleReadCharacter(const ByteCode &C) {
  /* read character */
  /* 12 */
  /* $v0 contains character read */
  HandleRead(MipsSyscallCode::READ_CHARACTER);
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
            JumpTargetLabel(TheContext.getName(), C.getIntOperand(),
                            /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::HandleJumpForward(const ByteCode &C) {
  WriteLine("j", JumpTargetLabel(TheContext.getName(), C.getIntOperand(),
                                 /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::HandleBinaryJumpIf(const char *Op,
                                                  const ByteCode &C) {
  POP("$t0"); // TOS
  POP("$t1"); // TOS1
  JumpTargetLabel Label(TheContext.getName(), C.getIntOperand(),
                        /* NeedColon */ false);
  WriteLine(Op, "$t1, $t0,", Label);
}

/// ByteCodeToMipsTranslator::Wrap OpcodeDispatcher::dispatch() to provide label
/// generation.
void ByteCodeToMipsTranslator::Write(const ByteCode &C) {
  unsigned Off = C.getByteCodeOffset();
  /// If this is a JumpTarget, emit a label.
  if (TheContext.IsJumpTarget(Off)) {
    WriteLine(JumpTargetLabel(TheContext.getName(), Off, /* NeedColon */ true));
  }
  WriteLine("#", C.getOpcodeName());
  OpcodeDispatcher::dispatch(C);
  WriteLine();
}

void ByteCodeToMipsTranslator::HandleJumpIfTrue(const ByteCode &C) {
  HandleUnaryJumpIf("bnez", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfFalse(const ByteCode &C) {
  HandleUnaryJumpIf("beqz", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfNotEqual(const ByteCode &C) {
  HandleBinaryJumpIf("bne", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfEqual(const ByteCode &C) {
  HandleBinaryJumpIf("beq", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfGreater(const ByteCode &C) {
  HandleBinaryJumpIf("bgt", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfGreaterEqual(const ByteCode &C) {
  HandleBinaryJumpIf("bge", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfLess(const ByteCode &C) {
  HandleBinaryJumpIf("blt", C);
}
void ByteCodeToMipsTranslator::HandleJumpIfLessEqual(const ByteCode &C) {
  HandleBinaryJumpIf("ble", C);
}

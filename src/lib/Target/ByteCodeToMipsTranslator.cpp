#include "simplecc/Target/ByteCodeToMipsTranslator.h"
#include "simplecc/Target/LocalContext.h"
#include <cassert>

using namespace simplecc;

// Push a register onto the stack
void ByteCodeToMipsTranslator::PUSH(const char *R) {
  // $sp points to the one entry pass TOS
  assert(R);
  assert(R[0] == '$');
  WriteLine("sw", R, ", 0($sp)");
  WriteLine("addiu $sp, $sp, -4");
  ++StackDepth;
}

// Pop the stack, optionally taking the tos value
void ByteCodeToMipsTranslator::POP(const char *R) {
  assert(StackDepth > 0 && "POP an empty stack!");
  WriteLine("addiu $sp, $sp, 4");
  if (R) {
    assert(R[0] == '$');
    WriteLine("lw", R, ", 0($sp)");
  }
  --StackDepth;
}

void ByteCodeToMipsTranslator::visitBinarySubscr(const ByteCode &C) {
  // elements of smaller index are stored at higher address
  // as opposed to C convention.
  POP("$t0");                     // index
  POP("$t1");                     // base
  WriteLine("sll $t0, $t0, 2");   // offset = index * 4
  WriteLine("addu $t2, $t1, $t0"); // address = base + offset
  WriteLine("lw $t3, 0($t2)");    // t3 = array[index]
  PUSH("$t3");
}

void ByteCodeToMipsTranslator::visitLoadLocal(const ByteCode &C) {
  auto offset = TheContext.getLocalOffset(C.getStrOperand());
  if (TheContext.IsVariable(C.getStrOperand())) {
    WriteLine("lw $t0,", offset, "($fp)");
  } else {
    WriteLine("addiu $t0, $fp,", offset);
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::visitLoadGlobal(const ByteCode &C) {
  GlobalLabel GL(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", GL);
  if (TheContext.IsVariable(C.getStrOperand())) {
    WriteLine("lw $t0, 0($t0)");
  }
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::visitLoadConst(const ByteCode &C) {
  WriteLine("li $t0,", C.getIntOperand());
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::visitLoadString(const ByteCode &C) {
  AsciizLabel AL(C.getIntOperand(), /* NeedColon */ false);
  WriteLine("la $t0,", AL);
  PUSH("$t0");
}

void ByteCodeToMipsTranslator::visitStoreGlobal(const ByteCode &C) {
  POP("$t0");
  GlobalLabel GL(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("sw $t0,", GL);
}

void ByteCodeToMipsTranslator::visitStoreLocal(const ByteCode &C) {
  POP("$t0");
  auto offset = TheContext.getLocalOffset(C.getStrOperand());
  WriteLine("sw $t0,", offset, "($fp)");
}

void ByteCodeToMipsTranslator::visitBinary(const char *op) {
  POP("$t0"); // TOS
  POP("$t1"); // TOS1
  WriteLine(op, "$t2, $t1, $t0");
  PUSH("$t2");
}

void ByteCodeToMipsTranslator::visitBinaryAdd(const ByteCode &C) {
  visitBinary("addu");
}
void ByteCodeToMipsTranslator::visitBinarySub(const ByteCode &C) {
  visitBinary("subu");
}
void ByteCodeToMipsTranslator::visitBinaryMultiply(const ByteCode &C) {
  visitBinary("mul");
}
void ByteCodeToMipsTranslator::visitBinaryDivide(const ByteCode &C) {
  visitBinary("div");
}

void ByteCodeToMipsTranslator::visitUnaryNegative(const ByteCode &C) {
  // $sp points to **the next vacant byte** of the stack, so
  // TOS is 4 + $sp
  WriteLine("lw $t0, 4($sp)");
  WriteLine("subu $t0, $zero, $t0");
  WriteLine("sw, $t0, 4($sp)");
}

void ByteCodeToMipsTranslator::visitCallFunction(const ByteCode &C) {
  GlobalLabel Fn(C.getStrOperand(), /* NeedColon */ false);
  WriteLine("jal", Fn);
  auto bytes = BytesFromEntries(C.getIntOperand());
  if (bytes) {
    WriteLine("addiu $sp, $sp", bytes);
  }
  PUSH("$v0");
}

void ByteCodeToMipsTranslator::visitReturn() {
  WriteLine("j", ReturnLabel(TheContext.getName(), /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::visitReturnValue(const ByteCode &C) {
  POP("$v0");
  visitReturn();
}

void ByteCodeToMipsTranslator::visitPrint(MipsSyscallCode Syscall) {
  WriteLine("li $v0,", Syscall);
  POP("$a0");
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::visitPrintString(const ByteCode &C) {
  /* print string */
  /* v0 = 4 */
  /* $a0 = address of null-terminated string to print */
  visitPrint(MipsSyscallCode::PRINT_STRING);
}

void ByteCodeToMipsTranslator::visitPrintCharacter(const ByteCode &C) {
  /* print character */
  /* 11 */
  /* $a0 = character to print */
  visitPrint(MipsSyscallCode::PRINT_CHARACTER);
}

void ByteCodeToMipsTranslator::visitPrintInteger(const ByteCode &C) {
  /* print integer */
  /* 1 */
  /* $a0 = integer to print */
  visitPrint(MipsSyscallCode::PRINT_INTEGER);
}

void ByteCodeToMipsTranslator::visitPrintNewline(const ByteCode &C) {
  WriteLine("li $a0,", static_cast<int>('\n'));
  WriteLine("li $v0,", MipsSyscallCode::PRINT_CHARACTER);
  WriteLine("syscall");
}

void ByteCodeToMipsTranslator::visitRead(MipsSyscallCode Syscall) {
  WriteLine("li $v0,", Syscall);
  WriteLine("syscall");
  PUSH("$v0");
}

void ByteCodeToMipsTranslator::visitReadInteger(const ByteCode &C) {
  /* read integer */
  /* 5 */
  /* $v0 contains integer read */
  visitRead(MipsSyscallCode::READ_INTEGER);
}

void ByteCodeToMipsTranslator::visitReadCharacter(const ByteCode &C) {
  /* read character */
  /* 12 */
  /* $v0 contains character read */
  visitRead(MipsSyscallCode::READ_CHARACTER);
}

void ByteCodeToMipsTranslator::visitStoreSubscr(const ByteCode &C) {
  POP("$t0");
  POP("$t1");
  POP("$t3");
  WriteLine("sll $t0, $t0, 2");
  WriteLine("addu $t2, $t1, $t0");
  WriteLine("sw $t3, 0($t2)");
}

void ByteCodeToMipsTranslator::visitUnaryJumpIf(const char *op,
                                                const ByteCode &C) {
  POP("$t0");
  WriteLine(op, "$t0",
            JumpTargetLabel(TheContext.getName(), C.getIntOperand(),
                            /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::visitJumpForward(const ByteCode &C) {
  WriteLine("j", JumpTargetLabel(TheContext.getName(), C.getIntOperand(),
                                 /* NeedColon */ false));
}

void ByteCodeToMipsTranslator::visitBinaryJumpIf(const char *Op,
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
  ByteCodeVisitor::visit(C);
  WriteLine();
}

void ByteCodeToMipsTranslator::visitJumpIfTrue(const ByteCode &C) {
  visitUnaryJumpIf("bnez", C);
}
void ByteCodeToMipsTranslator::visitJumpIfFalse(const ByteCode &C) {
  visitUnaryJumpIf("beqz", C);
}
void ByteCodeToMipsTranslator::visitJumpIfNotEqual(const ByteCode &C) {
  visitBinaryJumpIf("bne", C);
}
void ByteCodeToMipsTranslator::visitJumpIfEqual(const ByteCode &C) {
  visitBinaryJumpIf("beq", C);
}
void ByteCodeToMipsTranslator::visitJumpIfGreater(const ByteCode &C) {
  visitBinaryJumpIf("bgt", C);
}
void ByteCodeToMipsTranslator::visitJumpIfGreaterEqual(const ByteCode &C) {
  visitBinaryJumpIf("bge", C);
}
void ByteCodeToMipsTranslator::visitJumpIfLess(const ByteCode &C) {
  visitBinaryJumpIf("blt", C);
}
void ByteCodeToMipsTranslator::visitJumpIfLessEqual(const ByteCode &C) {
  visitBinaryJumpIf("ble", C);
}

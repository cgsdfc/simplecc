#include "Assemble.h"
#include "ByteCodeFunction.h"
#include "ByteCodeModule.h"
#include "MipsSupport.h"
#include "OpcodeDispatcher.h"
#include "Print.h"

#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace simplecompiler;

void EscapedString::Format(std::ostream &O) const {
  for (char C : Str) {
    O << C;
    if (C == '\\')
      O << C;
  }
}

// Provide local information for ByteCodeToMipsTranslator
class LocalContext {

  /// Initialize the **local offset** dictionary for a function.
  /// The LocalOffsets is where local variables live on the stack.
  void InitializeLocalOffsets() {
    // offset points to the first vacant byte after storing
    // $ra and $fp. $ra is at 0($fp), $fp is at -4($fp)
    LocalOffsets.clear();
    signed Off = -BytesFromEntries(2);

    /// Allocate space for formal arguments.
    for (const SymbolEntry &Arg : TheFunction->GetFormalArguments()) {
      LocalOffsets.emplace(Arg.GetName(), Off);
      Off -= BytesFromEntries(1);
    }

    /// Allocate space for non-arg local variables.
    for (const SymbolEntry &Var : TheFunction->GetLocalVariables()) {
      if (Var.IsArray()) {
        Off -= BytesFromEntries(Var.AsArray().GetSize());
        LocalOffsets.emplace(Var.GetName(), Off + BytesFromEntries(1));
        continue;
      }
      /// Variable:
      assert(Var.IsVariable());
      LocalOffsets.emplace(Var.GetName(), Off);
      Off -= BytesFromEntries(1);
    }
  }

  /// Initialize the **jump targets** set, which tells us whether an offset
  /// in the ByteCode stream is a target of some jump command.
  void InitializeJumpTargets() {
    JumpTargets.clear();
    for (const ByteCode &C : *TheFunction) {
      if (C.IsJumpXXX()) {
        JumpTargets.insert(C.GetIntOperand());
      }
    }
  }

public:
  LocalContext() = default;
  ~LocalContext() = default;

  /// Initialize both LocalOffsets and JumpTargets.
  void Initialize(const ByteCodeFunction &F) {
    TheFunction = &F;
    InitializeLocalOffsets();
    InitializeJumpTargets();
  }

  // Return if an offset is a jump target
  bool IsJumpTarget(unsigned Off) const { return JumpTargets.count(Off); }

  // Return the offset of local name relatited to frame pointer
  signed int GetLocalOffset(const char *Name) const {
    assert(LocalOffsets.count(Name) && "Undefined Name");
    return LocalOffsets.find(Name)->second;
  }

  // Return whether a name is a variable
  bool IsVariable(const char *Name) const {
    return TheFunction->GetLocal()[Name].IsVariable();
  }

  // Return whether a name is an array
  bool IsArray(const char *Name) const {
    return TheFunction->GetLocal()[Name].IsArray();
  }

  const String &getName() const { return TheFunction->getName(); }

private:
  std::unordered_map<String, signed> LocalOffsets;
  std::unordered_set<unsigned> JumpTargets;
  const ByteCodeFunction *TheFunction = nullptr;
};

// Serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator
    : OpcodeDispatcher<ByteCodeToMipsTranslator> { // {{{

  // Push a register onto the stack
  void PUSH(const char *r) {
    // $sp points to the one entry pass TOS
    assert(r);
    assert(r[0] == '$');
    WriteLine("sw", r, ", 0($sp)");
    WriteLine("addi $sp, $sp, -4");
    ++stack_level;
  }

  // Pop the stack, optionally taking the tos value
  void POP(const char *r = nullptr) {
    assert(stack_level > 0 && "POP an empty stack!");
    WriteLine("addi $sp, $sp, 4");
    if (r) {
      assert(r[0] == '$');
      WriteLine("lw", r, ", 0($sp)");
    }
    --stack_level;
  }

  void HandleLoadLocal(const ByteCode &C) {
    auto offset = TheContext.GetLocalOffset(C.GetStrOperand());
    if (TheContext.IsVariable(C.GetStrOperand())) {
      WriteLine("lw $t0,", offset, "($fp)");
    } else {
      WriteLine("addi $t0, $fp,", offset);
    }
    PUSH("$t0");
  }

  void HandleLoadGlobal(const ByteCode &C) {
    GlobalLabel GL(C.GetStrOperand(), /* NeedColon */ false);
    WriteLine("la $t0,", GL);
    if (TheContext.IsVariable(C.GetStrOperand())) {
      WriteLine("lw $t0, 0($t0)");
    }
    PUSH("$t0");
  }

  void HandleLoadConst(const ByteCode &C) {
    WriteLine("li $t0,", C.GetIntOperand());
    PUSH("$t0");
  }

  void HandleLoadString(const ByteCode &C) {
    AsciizLabel AL(C.GetIntOperand(), /* NeedColon */ false);
    WriteLine("la $t0,", AL);
    PUSH("$t0");
  }

  void HandleStoreLocal(const ByteCode &C) {
    POP("$t0");
    auto offset = TheContext.GetLocalOffset(C.GetStrOperand());
    WriteLine("sw $t0,", offset, "($fp)");
  }

  void HandleStoreGlobal(const ByteCode &C) {
    POP("$t0");
    GlobalLabel GL(C.GetStrOperand(), /* NeedColon */ false);
    WriteLine("sw $t0,", GL);
  }

  void HandleBinary(const char *op) {
    POP("$t0"); // TOS
    POP("$t1"); // TOS1
    WriteLine(op, "$t2, $t1, $t0");
    PUSH("$t2");
  }

  void HandleBinaryAdd(const ByteCode &C) { HandleBinary("add"); }

  void HandleBinarySub(const ByteCode &C) { HandleBinary("sub"); }

  void HandleBinaryMultiply(const ByteCode &C) { HandleBinary("mul"); }

  void HandleBinaryDivide(const ByteCode &C) { HandleBinary("div"); }

  void HandleUnaryPositive(const ByteCode &C) {
    // Nop
  }

  void HandleUnaryNegative(const ByteCode &C) {
    // $sp points to **the next vacant byte** of the stack, so
    // TOS is 4 + $sp
    WriteLine("lw $t0, 4($sp)");
    WriteLine("sub $t0, $zero, $t0");
    WriteLine("sw, $t0, 4($sp)");
  }

  void HandleCallFunction(const ByteCode &C) {
    GlobalLabel Fn(C.GetStrOperand(), /* NeedColon */ false);
    WriteLine("jal", Fn);
    auto bytes = BytesFromEntries(C.GetIntOperand());
    if (bytes) {
      WriteLine("addi $sp, $sp", bytes);
    }
    PUSH("$v0");
  }

  void HandleReturn() {
    WriteLine("j", ReturnLabel(TheContext.getName(), /* NeedColon */ false));
  }

  void HandleReturnValue(const ByteCode &C) {
    POP("$v0");
    HandleReturn();
  }

  void HandleReturnNone(const ByteCode &C) { HandleReturn(); }

  void HandlePrint(MipsSyscallNumber syscall_code) {
    WriteLine("li $v0,", syscall_code);
    POP("$a0");
    WriteLine("syscall");
  }

  void HandlePrintString(const ByteCode &C) {
    /* print string */
    /* v0 = 4 */
    /* $a0 = address of null-terminated string to print */
    HandlePrint(MipsSyscallNumber::PRINT_STRING);
  }

  void HandlePrintCharacter(const ByteCode &C) {
    /* print character */
    /* 11 */
    /* $a0 = character to print */
    HandlePrint(MipsSyscallNumber::PRINT_CHARACTER);
  }

  void HandlePrintInteger(const ByteCode &C) {
    /* print integer */
    /* 1 */
    /* $a0 = integer to print */
    HandlePrint(MipsSyscallNumber::PRINT_INTEGER);
  }

  void HandlePrintNewline(const ByteCode &C) {
    WriteLine("li $a0,", static_cast<int>('\n'));
    WriteLine("li $v0,", MipsSyscallNumber::PRINT_CHARACTER);
    WriteLine("syscall");
  }

  void HandleRead(MipsSyscallNumber syscall_code) {
    WriteLine("li $v0,", syscall_code);
    WriteLine("syscall");
    PUSH("$v0");
  }

  void HandleReadInteger(const ByteCode &C) {
    /* read integer */
    /* 5 */
    /* $v0 contains integer read */
    HandleRead(MipsSyscallNumber::READ_INTEGER);
  }

  void HandleReadCharacter(const ByteCode &C) {
    /* read character */
    /* 12 */
    /* $v0 contains character read */
    HandleRead(MipsSyscallNumber::READ_CHARACTER);
  }

  void HandleBinarySubscr(const ByteCode &C) {
    // elements of smaller index are stored at higher address
    // as opposed to C convension.
    POP("$t0");                     // index
    POP("$t1");                     // base
    WriteLine("sll $t0, $t0, 2");   // offset = index * 4
    WriteLine("add $t2, $t1, $t0"); // address = base + offset
    WriteLine("lw $t3, 0($t2)");    // t3 = array[index]
    PUSH("$t3");
  }

  void HandleStoreSubscr(const ByteCode &C) {
    POP("$t0");
    POP("$t1");
    POP("$t3");
    WriteLine("sll $t0, $t0, 2");
    WriteLine("add $t2, $t1, $t0");
    WriteLine("sw $t3, 0($t2)");
  }

  void HandleUnaryJumpIf(const char *op, const ByteCode &C) {
    POP("$t0");
    WriteLine(op, "$t0",
              JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                              /* NeedColon */ false));
  }

  void HandleJumpIfTrue(const ByteCode &C) { HandleUnaryJumpIf("bnez", C); }

  void HandleJumpIfFalse(const ByteCode &C) { HandleUnaryJumpIf("beqz", C); }

  void HandleJumpForward(const ByteCode &C) {
    WriteLine("j", JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                                   /* NeedColon */ false));
  }

  void HandleBinaryJumpIf(const char *Op, const ByteCode &C) {
    POP("$t0"); // TOS
    POP("$t1"); // TOS1
    WriteLine(Op, "$t1, $t0,",
              JumpTargetLabel(TheContext.getName(), C.GetIntOperand(),
                              /* NeedColon */ false));
  }

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
  void Write(const ByteCode &C) {
    unsigned Off = C.GetByteCodeOffset();
    /// If this is a JumpTarget, emit a label.
    if (TheContext.IsJumpTarget(Off)) {
      WriteLine(
          JumpTargetLabel(TheContext.getName(), Off, /* NeedColon */ true));
    }
    WriteLine("#", C.GetOpcodeName());
    OpcodeDispatcher::dispatch(C);
    WriteLine();
  }

private:
  friend class OpcodeDispatcher<ByteCodeToMipsTranslator>;
  Printer ThePrinter;
  const LocalContext &TheContext;
  int stack_level = 0;
};
// }}}

namespace simplecompiler {
class MipsAssemblyWriter {

  static constexpr int InBytes(int Entries) { return 4 * Entries; }

  void WriteData(Printer &W, const ByteCodeModule &Module) {
    W.WriteLine(".data");
    W.WriteLine("# Global objects");

    for (const SymbolEntry &E : Module.GetGlobalVariables()) {
      GlobalLabel GL(E.GetName(), false);
      if (!E.IsArray()) {
        W.WriteLine(GL, ".word", 0);
        continue;
      }
      W.WriteLine(GL, ".space", BytesFromEntries(E.AsArray().GetSize()));
    }

    W.WriteLine();
    W.WriteLine("# String literals");

    for (const std::pair<String, unsigned> &Item :
         Module.GetStringLiteralTable()) {
      W.WriteLine(AsciizLabel(Item.second, /* NeedColon */ true), ".asciiz",
                  EscapedString(Item.first));
    }

    W.WriteLine("# End of data segment");
  }

  // Return the total bytes consumed by local objects, including
  // variables, arrays and formal arguments.
  unsigned GetLocalObjectsInBytes(const ByteCodeFunction &TheFunction) const {
    unsigned Entries = std::accumulate(
        TheFunction.local_begin(), TheFunction.local_end(),
        TheFunction.GetFormalArgumentCount(),
        [](unsigned Entries, const SymbolEntry &E) {
          return Entries + (E.IsArray() ? E.AsArray().GetSize() : 1);
        });
    return InBytes(Entries);
  }

  void WriteFunction(Printer &W, const ByteCodeFunction &TheFunction) {
    TheContext.Initialize(TheFunction);
    ByteCodeToMipsTranslator TheTranslator(W.getOuts(), TheContext);

    WritePrologue(W, TheFunction);
    for (const ByteCode &C : TheFunction) {
      TheTranslator.Write(C);
    }
    WriteEpilogue(W, TheFunction);
  }

  void WriteText(Printer &W, const ByteCodeModule &Module) {
    W.WriteLine(".text");
    W.WriteLine(".globl main");
    W.WriteLine("jal main");
    W.WriteLine("li $v0,", MipsSyscallNumber::EXIT_PROGRAM);
    W.WriteLine("syscall");
    W.WriteLine();
    W.WriteLine("# User defined functions");

    for (const ByteCodeFunction *Fn : Module) {
      WriteFunction(W, *Fn);
      W.WriteLine();
    }
    W.WriteLine("# End of text segment");
  }

  void WritePrologue(Printer &W, const ByteCodeFunction &TheFunction) {
    W.WriteLine(GlobalLabel(TheFunction.getName(), /* NeedColon */ true));
    W.WriteLine("# Prologue");
    W.WriteLine("sw $ra, 0($sp)");
    W.WriteLine("sw $fp, -4($sp)");
    W.WriteLine("move $fp, $sp");
    W.WriteLine("addi $sp, $sp,", -BytesFromEntries(2));
    W.WriteLine();

    auto NumArgs = TheFunction.GetFormalArgumentCount();
    if (NumArgs) {
      // copy arguments here
      W.WriteLine("# Passing Arguments");
      for (int I = 0; I < NumArgs; I++) {
        // actual is above $fp, with $fp + 4 pointing to the last arg.
        signed int Actual = BytesFromEntries(NumArgs - I);
        // formal is under $sp, with $sp pointing to the first arg.
        signed int Formal = BytesFromEntries(-I);
        W.WriteLine("lw $t0,", Actual, "($fp)");
        W.WriteLine("sw $t0,", Formal, "($sp)");
      }
      W.WriteLine();
    }

    signed Off = GetLocalObjectsInBytes(TheFunction);
    if (Off != 0) {
      W.WriteLine("# Make room for local objects");
      W.WriteLine("addi $sp, $sp,", -Off);
      W.WriteLine();
    }
    // now $fp points to the bottom of stack,
    // $sp points to the next TOS of the stack.
    // $ra stored at 0($fp)
    // $fp stored at -4($fp)
  }

  void WriteEpilogue(Printer &W, const ByteCodeFunction &TheFunction) {
    W.WriteLine("# Epilogue");
    W.WriteLine(ReturnLabel(TheFunction.getName(), /* NeedColon */ true));
    W.WriteLine("lw $ra, 0($fp)");
    W.WriteLine("move $sp, $fp");
    W.WriteLine("lw $fp, -4($fp)");
    W.WriteLine("jr $ra");
  }

public:
  MipsAssemblyWriter() = default;
  ~MipsAssemblyWriter() = default;

  void Write(const ByteCodeModule &M, std::ostream &O) {
    Printer W(O);
    WriteData(W, M);
    W.WriteLine();
    WriteText(W, M);
  }

private:
  LocalContext TheContext;
};

void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

} // namespace simplecompiler

#include "Assemble.h"
#include "ByteCodeFunction.h"
#include "ByteCodeModule.h"
#include "OpcodeDispatcher.h"
#include "Print.h"

#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace simplecompiler;

/// CRTP base for label.
template <typename Derived> class LabelBase {
protected:
  /// Whether this label needs a colon at the end.
  bool NeedColon;

public:
  LabelBase(bool N) : NeedColon(N) {}
  void setNeedColon(bool B) { NeedColon = B; }
  void Format(std::ostream &O) const {
    static_cast<const Derived *>(this)->FormatImpl(O);
    if (NeedColon)
      O << ":";
  }
};

template <typename Derived>
std::ostream &operator<<(std::ostream &O, const LabelBase<Derived> &LB) {
  LB.Format(O);
  return O;
}

class AsciizLabel : public LabelBase<AsciizLabel> {
  unsigned StringID;

public:
  AsciizLabel(unsigned N, bool NeedColon) : LabelBase(NeedColon), StringID(N) {}
  void FormatImpl(std::ostream &O) const { O << "string_" << StringID; }
};

class JumpTargetLabel : public LabelBase<JumpTargetLabel> {
  const char *ParentName;
  unsigned Target;

public:
  JumpTargetLabel(const char *PN, unsigned T, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN), Target(T) {}
  JumpTargetLabel(const String &PN, unsigned T, bool NeedColon)
      : JumpTargetLabel(PN.data(), T, NeedColon) {}

  void FormatImpl(std::ostream &O) const {
    O << ParentName << "_label_" << Target;
  }
};

class ReturnLabel : public LabelBase<ReturnLabel> {
  const char *ParentName;

public:
  ReturnLabel(const String &PN, bool NeedColon)
      : LabelBase(NeedColon), ParentName(PN.data()) {}
  void FormatImpl(std::ostream &O) const { O << ParentName << "return"; }
};

/// A Global label that is unique with the module.
class GlobalLabel : public LabelBase<GlobalLabel> {
  const char *Name;

public:
  GlobalLabel(const char *N, bool NeedColon) : LabelBase(NeedColon), Name(N) {}
  GlobalLabel(const String &N, bool NeedColon)
      : GlobalLabel(N.data(), NeedColon) {}

  void FormatImpl(std::ostream &O) const { O << Name; }
};

/// A wrapper class that escapes the string when Format'ted.
class EscapedString {
  const String &Str;

public:
  EscapedString(const String &S) : Str(S) {}

  void Format(std::ostream &O) const {
    for (char C : Str) {
      O << C;
      if (C == '\\')
        O << C;
    }
  }
};

std::ostream &operator<<(std::ostream &O, const EscapedString &E) {
  E.Format(O);
  return O;
}

// Return the bytes for n entries
inline constexpr int BytesFromEntries(int n_entries) { return 4 * n_entries; }

// Provide local information for ByteCodeToMipsTranslator
class LocalContext {
  // byte offset of local objects relatited to $fp
  std::unordered_map<String, int> local_offsets;
  // a byte code offset is a jump target if in it
  std::unordered_set<int> jump_targets;
  // name of the function being translated
  const ByteCodeFunction *TheFunction;

  // populate local_offsets
  void MakeLocalOffsets() {
    local_offsets.clear();
    // offset points to the first vacant byte after storing
    // $ra and $fp. $ra is at 0($fp), $fp is at -4($fp)
    auto offset = BytesFromEntries(-2);
    for (const auto &arg : TheFunction->GetFormalArguments()) {
      local_offsets.emplace(arg.GetName(), offset);
      offset -= BytesFromEntries(1);
    }
    for (const auto &obj : TheFunction->GetLocalVariables()) {
      if (obj.IsArray()) {
        offset -= BytesFromEntries(obj.AsArray().GetSize());
        local_offsets.emplace(obj.GetName(), offset + BytesFromEntries(1));
      } else {
        assert(obj.IsVariable());
        local_offsets.emplace(obj.GetName(), offset);
        offset -= BytesFromEntries(1);
      }
    }
  }

  // populate jump_targets
  void MakeJumpTargets() {
    jump_targets.clear();
    for (const auto &code : *TheFunction) {
      if (code.IsJumpXXX()) {
        jump_targets.insert(code.GetIntOperand());
      }
    }
  }

public:
  LocalContext() = default;

  void Initialize(const ByteCodeFunction &fun) {
    TheFunction = &fun;
    MakeLocalOffsets();
    MakeJumpTargets();
  }

  // Return if an offset is a jump target
  bool IsTarget(int offset) const { return jump_targets.count(offset); }

  // Return the offset of local name relatited to frame pointer
  int GetLocalOffset(const char *name) const { return local_offsets.at(name); }

  // Return whether a name is a variable
  bool IsVariable(const char *Name) const {
    return TheFunction->GetLocal()[Name].IsVariable();
  }

  // Return whether a name is an array
  bool IsArray(const char *Name) const {
    return TheFunction->GetLocal()[Name].IsArray();
  }

  const String &getName() const { return TheFunction->getName(); }

};

enum class MipsSyscallNumber {
  PRINT_STRING = 4,
  PRINT_CHARACTER = 11,
  PRINT_INTEGER = 1,
  READ_INTEGER = 5,
  READ_CHARACTER = 12,
  EXIT_PROGRAM = 10,
};

inline std::ostream &operator<<(std::ostream &os, MipsSyscallNumber syscall) {
  return os << static_cast<int>(syscall);
}

// Serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator
    : public OpcodeDispatcher<ByteCodeToMipsTranslator> { // {{{
public:
  ByteCodeToMipsTranslator(Printer &w, const LocalContext &context)
      : w(w), context(context) {}

  // Push a register onto the stack
  void PUSH(const char *r) {
    // $sp points to the one entry pass TOS
    assert(r);
    assert(r[0] == '$');
    w.WriteLine("sw", r, ", 0($sp)");
    w.WriteLine("addi $sp, $sp, -4");
    ++stack_level;
  }

  // Pop the stack, optionally taking the tos value
  void POP(const char *r = nullptr) {
    assert(stack_level > 0 && "POP an empty stack!");
    w.WriteLine("addi $sp, $sp, 4");
    if (r) {
      assert(r[0] == '$');
      w.WriteLine("lw", r, ", 0($sp)");
    }
    --stack_level;
  }

  void HandleLoadLocal(const ByteCode &code) {
    auto offset = context.GetLocalOffset(code.GetStrOperand());
    if (context.IsVariable(code.GetStrOperand())) {
      w.WriteLine("lw $t0,", offset, "($fp)");
    } else {
      w.WriteLine("addi $t0, $fp,", offset);
    }
    PUSH("$t0");
  }

  void HandleLoadGlobal(const ByteCode &code) {
    GlobalLabel GL(code.GetStrOperand(), /* NeedColon */ false);
    w.WriteLine("la $t0,", GL);
    if (context.IsVariable(code.GetStrOperand())) {
      w.WriteLine("lw $t0, 0($t0)");
    }
    PUSH("$t0");
  }

  void HandleLoadConst(const ByteCode &code) {
    w.WriteLine("li $t0,", code.GetIntOperand());
    PUSH("$t0");
  }

  void HandleLoadString(const ByteCode &code) {
    AsciizLabel AL(code.GetIntOperand(), /* NeedColon */ false);
    w.WriteLine("la $t0,", AL);
    PUSH("$t0");
  }

  void HandleStoreLocal(const ByteCode &code) {
    POP("$t0");
    auto offset = context.GetLocalOffset(code.GetStrOperand());
    w.WriteLine("sw $t0,", offset, "($fp)");
  }

  void HandleStoreGlobal(const ByteCode &code) {
    POP("$t0");
    GlobalLabel GL(code.GetStrOperand(), /* NeedColon */ false);
    w.WriteLine("sw $t0,", GL);
  }

  void HandleBinary(const char *op) {
    POP("$t0"); // TOS
    POP("$t1"); // TOS1
    w.WriteLine(op, "$t2, $t1, $t0");
    PUSH("$t2");
  }

  void HandleBinaryAdd(const ByteCode &code) { HandleBinary("add"); }

  void HandleBinarySub(const ByteCode &code) { HandleBinary("sub"); }

  void HandleBinaryMultiply(const ByteCode &code) { HandleBinary("mul"); }

  void HandleBinaryDivide(const ByteCode &code) { HandleBinary("div"); }

  void HandleUnaryPositive(const ByteCode &code) {
    // Nop
  }

  void HandleUnaryNegative(const ByteCode &code) {
    // $sp points to **the next vacant byte** of the stack, so
    // TOS is 4 + $sp
    w.WriteLine("lw $t0, 4($sp)");
    w.WriteLine("sub $t0, $zero, $t0");
    w.WriteLine("sw, $t0, 4($sp)");
  }

  void HandleCallFunction(const ByteCode &code) {
    GlobalLabel Fn(code.GetStrOperand(), /* NeedColon */ false);
    w.WriteLine("jal", Fn);
    auto bytes = BytesFromEntries(code.GetIntOperand());
    if (bytes) {
      w.WriteLine("addi $sp, $sp", bytes);
    }
    PUSH("$v0");
  }

  void HandleReturn() {
    w.WriteLine("j", ReturnLabel(Name, /* NeedColon */ false));
  }

  void HandleReturnValue(const ByteCode &code) {
    POP("$v0");
    HandleReturn();
  }

  void HandleReturnNone(const ByteCode &code) { HandleReturn(); }

  void HandlePrint(MipsSyscallNumber syscall_code) {
    w.WriteLine("li $v0,", syscall_code);
    POP("$a0");
    w.WriteLine("syscall");
  }

  void HandlePrintString(const ByteCode &code) {
    /* print string */
    /* v0 = 4 */
    /* $a0 = address of null-terminated string to print */
    HandlePrint(MipsSyscallNumber::PRINT_STRING);
  }

  void HandlePrintCharacter(const ByteCode &code) {
    /* print character */
    /* 11 */
    /* $a0 = character to print */
    HandlePrint(MipsSyscallNumber::PRINT_CHARACTER);
  }

  void HandlePrintInteger(const ByteCode &code) {
    /* print integer */
    /* 1 */
    /* $a0 = integer to print */
    HandlePrint(MipsSyscallNumber::PRINT_INTEGER);
  }

  void HandlePrintNewline(const ByteCode &code) {
    w.WriteLine("li $a0,", static_cast<int>('\n'));
    w.WriteLine("li $v0,", MipsSyscallNumber::PRINT_CHARACTER);
    w.WriteLine("syscall");
  }

  void HandleRead(MipsSyscallNumber syscall_code) {
    w.WriteLine("li $v0,", syscall_code);
    w.WriteLine("syscall");
    PUSH("$v0");
  }

  void HandleReadInteger(const ByteCode &code) {
    /* read integer */
    /* 5 */
    /* $v0 contains integer read */
    HandleRead(MipsSyscallNumber::READ_INTEGER);
  }

  void HandleReadCharacter(const ByteCode &code) {
    /* read character */
    /* 12 */
    /* $v0 contains character read */
    HandleRead(MipsSyscallNumber::READ_CHARACTER);
  }

  void HandleBinarySubscr(const ByteCode &code) {
    // elements of smaller index are stored at higher address
    // as opposed to C convension.
    POP("$t0");                       // index
    POP("$t1");                       // base
    w.WriteLine("sll $t0, $t0, 2");   // offset = index * 4
    w.WriteLine("add $t2, $t1, $t0"); // address = base + offset
    w.WriteLine("lw $t3, 0($t2)");    // t3 = array[index]
    PUSH("$t3");
  }

  void HandleStoreSubscr(const ByteCode &code) {
    POP("$t0");
    POP("$t1");
    POP("$t3");
    w.WriteLine("sll $t0, $t0, 2");
    w.WriteLine("add $t2, $t1, $t0");
    w.WriteLine("sw $t3, 0($t2)");
  }

  void HandleUnaryJumpIf(const char *op, const ByteCode &code) {
    POP("$t0");
    w.WriteLine(
        op, "$t0",
        JumpTargetLabel(Name, code.GetIntOperand(), /* NeedColon */ false));
  }

  void HandleJumpIfTrue(const ByteCode &code) {
    HandleUnaryJumpIf("bnez", code);
  }

  void HandleJumpIfFalse(const ByteCode &code) {
    HandleUnaryJumpIf("beqz", code);
  }

  void HandleJumpForward(const ByteCode &code) {
    w.WriteLine("j", JumpTargetLabel(Name, code.GetIntOperand(),
                                     /* NeedColon */ false));
  }

  void HandleBinaryJumpIf(const char *op, const ByteCode &code) {
    POP("$t0"); // TOS
    POP("$t1"); // TOS1
    w.WriteLine(
        op, "$t1, $t0,",
        JumpTargetLabel(Name, code.GetIntOperand(), /* NeedColon */ false));
  }

  void HandleJumpIfNotEqual(const ByteCode &code) {
    HandleBinaryJumpIf("bne", code);
  }

  void HandleJumpIfEqual(const ByteCode &code) {
    HandleBinaryJumpIf("beq", code);
  }

  void HandleJumpIfGreater(const ByteCode &code) {
    HandleBinaryJumpIf("bgt", code);
  }

  void HandleJumpIfGreaterEqual(const ByteCode &code) {
    HandleBinaryJumpIf("bge", code);
  }

  void HandleJumpIfLess(const ByteCode &code) {
    HandleBinaryJumpIf("blt", code);
  }

  void HandleJumpIfLessEqual(const ByteCode &code) {
    HandleBinaryJumpIf("ble", code);
  }

  void HandlePopTop(const ByteCode &code) { POP(); }

private:
  String Name;
  Printer &w;
  const LocalContext &context;
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

  bool IsJumpTarget(unsigned Off) { return JumpTargets.count(Off); }

  void WriteFunction(Printer &W, const ByteCodeFunction &TheFunction) {
    InitializeLocalInfo(TheFunction);

    WritePrologue(W, TheFunction);
    for (const ByteCode &C : TheFunction) {
      unsigned Off = C.GetByteCodeOffset();
      if (IsJumpTarget(Off)) {
        W.WriteLine(
            JumpTargetLabel(TheFunction.getName(), Off, /* NeedColon */ true));
      }
      // ByteCodeToMipsTranslator:
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

    auto nargs = TheFunction.GetFormalArgumentCount();
    if (nargs) {
      // copy arguments here
      W.WriteLine("# Passing Arguments");
      for (int i = 0; i < nargs; i++) {
        // actual is above $fp, with $fp + 4 pointing to the last arg.
        auto actual = BytesFromEntries(nargs - i);
        // formal is under $sp, with $sp pointing to the first arg.
        auto formal = BytesFromEntries(-i);
        W.WriteLine("lw $t0,", actual, "($fp)");
        W.WriteLine("sw $t0,", formal, "($sp)");
      }
      W.WriteLine();
    }

    auto Off = GetLocalObjectsInBytes(TheFunction);
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

  /// Initialize the **local offset** dictionary for a function.
  /// The LocalOffsets is where local variables live on the stack.
  void InitializeLocalOffsets(const ByteCodeFunction &TheFunction) {
    // offset points to the first vacant byte after storing
    // $ra and $fp. $ra is at 0($fp), $fp is at -4($fp)
    LocalOffsets.clear();
    auto Off = -InBytes(2);

    /// Allocate space for formal arguments.
    for (const SymbolEntry &Arg : TheFunction.GetFormalArguments()) {
      LocalOffsets.emplace(Arg.GetName(), Off);
      Off -= InBytes(1);
    }

    /// Allocate space for non-arg local variables.
    for (const SymbolEntry &Var : TheFunction.GetLocalVariables()) {
      if (Var.IsArray()) {
        Off -= InBytes(Var.AsArray().GetSize());
        LocalOffsets.emplace(Var.GetName(), Off + InBytes(1));
        continue;
      }
      /// Variable:
      assert(Var.IsVariable());
      LocalOffsets.emplace(Var.GetName(), Off);
      Off -= InBytes(1);
    }
  }

  /// Initialize the **jump targets** set, which tells us whether an offset
  /// in the ByteCode stream is a target of some jump command.
  void InitializeJumpTargets(const ByteCodeFunction &TheFunction) {
    JumpTargets.clear();
    for (const ByteCode &C : TheFunction) {
      if (C.IsJumpXXX()) {
        JumpTargets.insert(C.GetIntOperand());
      }
    }
  }

  /// Initialize both LocalOffsets and JumpTargets.
  void InitializeLocalInfo(const ByteCodeFunction &TheFunction) {
    InitializeLocalOffsets(TheFunction);
    InitializeJumpTargets(TheFunction);
  }

  void WriteByteCode(Printer &W, const ByteCode &C);
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
  std::unordered_map<String, unsigned> LocalOffsets;
  std::unordered_set<unsigned> JumpTargets;
};

void AssembleMips(const ByteCodeModule &M, std::ostream &O) {
  MipsAssemblyWriter().Write(M, O);
}

}

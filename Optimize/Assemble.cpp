#include "Assemble.h"
#include "Compile.h"
#include "OpcodeDispatcher.h"
#include "Print.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace simplecompiler;
namespace {

// Return the bytes for n entries
inline constexpr int BytesFromEntries(int n_entries) { return 4 * n_entries; }

// Handle global object naming
class GlobalContext {
public:
  // Return a label for a global name
  static String GetGlobalLabel(const String &name, bool colon) {
    return colon ? name + ":" : name;
  }

  // Return a label for a string literal
  static String GetStringLiteralLabel(int ID, bool colon) {
    std::ostringstream os;
    os << "string_" << ID;
    if (colon)
      os << ":";
    return os.str();
  }
};

// Provide local information for ByteCodeToMipsTranslator
class LocalContext {
  // byte offset of local objects relatited to $fp
  std::unordered_map<String, int> local_offsets;
  // a byte code offset is a jump target if in it
  std::unordered_set<int> jump_targets;
  // name of the function being translated
  const String &name;
  // local symbols for type information
  SymbolTableView local;

  // populate local_offsets
  void MakeLocalOffsets(const CompiledFunction &fun) {
    // offset points to the first vacant byte after storing
    // $ra and $fp. $ra is at 0($fp), $fp is at -4($fp)
    auto offset = BytesFromEntries(-2);
    for (const auto &arg : fun.GetFormalArguments()) {
      local_offsets.emplace(arg.GetName(), offset);
      offset -= BytesFromEntries(1);
    }
    for (const auto &obj : fun.GetLocalObjects()) {
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
  void MakeJumpTargets(const CompiledFunction &fun) {
    for (const auto &code : fun) {
      if (IsJumpXXX(code.GetOpcode())) {
        jump_targets.insert(code.GetIntOperand());
      }
    }
  }

public:
  LocalContext(const CompiledFunction &fun)
      : local_offsets(), jump_targets(), name(fun.getName()),
        local(fun.GetLocal()) {
    MakeLocalOffsets(fun);
    MakeJumpTargets(fun);
  }

  // Return if an offset is a jump target
  bool IsTarget(int offset) const { return jump_targets.count(offset); }

  // Return the offset of local name relatited to frame pointer
  int GetLocalOffset(const char *name) const { return local_offsets.at(name); }

  // Return the label denoting a global object.
  String GetGlobalLabel(const char *name) const {
    return GlobalContext::GetGlobalLabel(name, false);
  }

  // Return the label denoting a string literal.
  String GetStringLiteralLabel(int ID) const {
    return GlobalContext::GetStringLiteralLabel(ID, false);
  }

  // Return the label denoting the epilogue of a function.
  String GetReturnLabel(bool colon) const {
    std::ostringstream os;
    os << name << "_return";
    if (colon)
      os << ":";
    return os.str();
  }

  // Return the label of a jump target of a function.
  String GetTargetLabel(int offset, bool colon = false) const {
    std::ostringstream os;
    os << name << "_label_" << offset;
    if (colon)
      os << ":";
    return os.str();
  }

  // Return whether a name is a variable
  bool IsVariable(const char *name) const { return local[name].IsVariable(); }

  // Return whether a name is an array
  bool IsArray(const char *name) const { return local[name].IsArray(); }
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
  Printer &w;
  const LocalContext &context;
  int stack_level = 0;

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
    auto &&label = context.GetGlobalLabel(code.GetStrOperand());
    w.WriteLine("la $t0,", label);
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
    w.WriteLine("la $t0,", context.GetStringLiteralLabel(code.GetIntOperand()));
    PUSH("$t0");
  }

  void HandleStoreLocal(const ByteCode &code) {
    POP("$t0");
    auto offset = context.GetLocalOffset(code.GetStrOperand());
    w.WriteLine("sw $t0,", offset, "($fp)");
  }

  void HandleStoreGlobal(const ByteCode &code) {
    POP("$t0");
    auto &&label = context.GetGlobalLabel(code.GetStrOperand());
    w.WriteLine("sw $t0,", label);
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
    auto &&label = context.GetGlobalLabel(code.GetStrOperand());
    w.WriteLine("jal", label);
    auto bytes = BytesFromEntries(code.GetIntOperand());
    if (bytes) {
      w.WriteLine("addi $sp, $sp", bytes);
    }
    PUSH("$v0");
  }

  void HandleReturn() {
    auto &&label = context.GetReturnLabel(false);
    w.WriteLine("j", label);
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
    auto &&label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine(op, "$t0", label);
  }

  void HandleJumpIfTrue(const ByteCode &code) {
    HandleUnaryJumpIf("bnez", code);
  }

  void HandleJumpIfFalse(const ByteCode &code) {
    HandleUnaryJumpIf("beqz", code);
  }

  void HandleJumpForward(const ByteCode &code) {
    auto &&label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine("j", label);
  }

  void HandleBinaryJumpIf(const char *op, const ByteCode &code) {
    POP("$t0"); // TOS
    POP("$t1"); // TOS1
    auto &&label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine(op, "$t1, $t0,", label);
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
};
// }}}

// Assemble a CompiledFunction to MIPS code
class FunctionAssembler {
  const CompiledFunction &source;
  Printer &w;
  LocalContext context;

  // Return the total bytes consumed by local objects, including
  // variables, arrays and formal arguments.
  int GetLocalObjectsBytes() const {
    auto entries = source.GetFormalArgumentCount();
    for (const auto &obj : source.GetLocalObjects()) {
      entries += obj.IsArray() ? obj.AsArray().GetSize() : 1;
    }
    return BytesFromEntries(entries);
  }

  void MakePrologue() {
    w.WriteLine("# Prologue");
    w.WriteLine("sw $ra, 0($sp)");
    w.WriteLine("sw $fp, -4($sp)");
    w.WriteLine("move $fp, $sp");
    w.WriteLine("addi $sp, $sp,", -BytesFromEntries(2));
    w.WriteLine();

    auto nargs = source.GetFormalArgumentCount();
    if (nargs) {
      // copy arguments here
      w.WriteLine("# Passing Arguments");
      for (int i = 0; i < nargs; i++) {
        // actual is above $fp, with $fp + 4 pointing to the last arg.
        auto actual = BytesFromEntries(nargs - i);
        // formal is under $sp, with $sp pointing to the first arg.
        auto formal = BytesFromEntries(-i);
        w.WriteLine("lw $t0,", actual, "($fp)");
        w.WriteLine("sw $t0,", formal, "($sp)");
      }
      w.WriteLine();
    }

    auto offset = -GetLocalObjectsBytes();
    if (offset) {
      w.WriteLine("# Make room for local objects");
      w.WriteLine("addi $sp, $sp,", offset);
      w.WriteLine();
    }
    // now $fp points to the bottom of stack,
    // $sp points to the next TOS of the stack.
    // $ra stored at 0($fp)
    // $fp stored at -4($fp)
  }

  void MakeEpilogue() {
    w.WriteLine("# Epilogue");
    w.WriteLine(context.GetReturnLabel(true));
    w.WriteLine("lw $ra, 0($fp)");
    w.WriteLine("move $sp, $fp");
    w.WriteLine("lw $fp, -4($fp)");
    w.WriteLine("jr $ra");
  }

public:
  FunctionAssembler(const CompiledFunction &source, Printer &w)
      : source(source), w(w), context(source) {}

  // public interface
  void Assemble() {
    ByteCodeToMipsTranslator translator(w, context);
    MakePrologue();
    for (const auto &byteCode : source) {
      auto offset = byteCode.GetOffset();
      if (context.IsTarget(offset)) {
        w.WriteLine(context.GetTargetLabel(offset, true));
      }
      // corresponding ByteCode
      w.WriteLine("#", byteCode);
      translator.dispatch(byteCode);
      w.WriteLine();
    }
    MakeEpilogue();
  }
};

// Assemble a whole MIPS program
class ModuleAssembler {
  const CompiledModule &module;
  Printer w;

  // For each '\\' in string, make it doubled
  static String DoubleBackslashes(const String &string) {
    String result;
    result.reserve(string.size());
    for (auto c : string) {
      result.push_back(c);
      if (c == '\\') {
        result.push_back(c);
      }
    }
    return std::move(result);
  }

  void MakeDataSegment() {
    w.WriteLine(".data");
    w.WriteLine("# Global objects");
    for (const auto &obj : module.GetGlobalObjects()) {
      auto &&label = GlobalContext::GetGlobalLabel(obj.GetName(), true);
      if (obj.IsArray()) {
        auto bytes = BytesFromEntries(obj.AsArray().GetSize());
        w.WriteLine(label, ".space", bytes);
      } else {
        w.WriteLine(label, ".word", 0);
      }
    }
    w.WriteLine();
    w.WriteLine("# String literals");
    for (const auto &item : module.GetStringLiteralTable()) {
      auto &&label = GlobalContext::GetStringLiteralLabel(item.second, true);
      w.WriteLine(label, ".asciiz", DoubleBackslashes(item.first));
    }
    w.WriteLine("# End of data segment");
  }

  void MakeTextSegment() {
    w.WriteLine(".text");
    w.WriteLine("# Boilerplate");
    w.WriteLine(".globl main");
    w.WriteLine("jal main");
    w.WriteLine("li $v0,", MipsSyscallNumber::EXIT_PROGRAM);
    w.WriteLine("syscall");
    w.WriteLine();
    w.WriteLine("# User defined functions");

    for (const auto &fun : module) {
      w.WriteLine(GlobalContext::GetGlobalLabel(fun->getName(), true));
      FunctionAssembler(*fun, w).Assemble();
      w.WriteLine("# End of", fun->getName());
      w.WriteLine();
    }
    w.WriteLine("# End of text segment");
  }

public:
  ModuleAssembler(const CompiledModule &module, std::ostream &os)
      : module(module), w(os) {}

  void Assemble() {
    MakeDataSegment();
    w.WriteLine();
    MakeTextSegment();
  }
};
} // namespace

void simplecompiler::AssembleMips(const CompiledModule &module,
                                  std::ostream &os) {
  ModuleAssembler(module, os).Assemble();
}

// vim: set foldmethod=marker

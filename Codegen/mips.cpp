#include "mips.h"
#include "OpcodeDispatcher.h"
#include "compile.h"
#include "error.h"

#include <unordered_set>

class AssemblyWriter {
  std::ostream &os;
public:
  AssemblyWriter(std::ostream &os): os(os) {}

  template <typename... Args>
  void WriteLine(Args&&... args) {
    Print(os, std::forward<Args>(args)...);
  }

};

// Provide information of varying bits in the template
class LocalContext {
public:
  std::unordered_map<const char*, int> variable_location;
  std::unordered_set<int> target_index;

  LocalContext() {}
  bool IsTarget(int offset) {
    return true;
  }

  int GetLocalOffset(const char *local) const {
    return 0;
  }

  String GetGlobalLabel(const char *name) const {
    return "";
  }

  String GetStringLiteralLable(int ID) const {
    return "";
  }

  String GetReturnLabel() const {
    return "";
  }

  int GetStackOffset(int n_entries) const {
    return 4 * n_entries;
  }

  String GetTargetLabel(int offset) const {
    return "";
  }

};

// serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator: public OpcodeDispatcher<ByteCodeToMipsTranslator> {
  AssemblyWriter &w;
  const LocalContext &context;

public:
  ByteCodeToMipsTranslator(AssemblyWriter &w, const LocalContext &context):
    w(w), context(context) {}

  void PUSH(const char *r) {
    /* # PUSH the item in $t0: */
    /* subu $sp,$sp,4      #   point to the place for the new item, */
    /* sw   $t0,($sp)      #   store the contents of $t0 as the new top. */
    w.WriteLine("subu $sp, $sp, 4");
    w.WriteLine("sw", r, "0($sp)");
  }

  void POP(const char *r = nullptr) {
    /* lw   $t0,($sp)      #   Copy top item to $t0. */
    /* addu $sp,$sp,4      #   Point to the item beneath the old top. */
    if (r) {
      w.WriteLine("lw", r, "0($sp)");
    }
    w.WriteLine("addu $sp, $sp, 4");
  }

  void HandleLoadLocal(const ByteCode &code) {
    auto offset = context.GetLocalOffset(code.GetStrOperand());
    w.WriteLine("lw $t0,", offset, "($fp)");
    PUSH("$t0");
  }

  void HandleLoadGlobal(const ByteCode &code) {
    auto label = context.GetGlobalLabel(code.GetStrOperand());
    w.WriteLine("la $t0,", label);
    w.WriteLine("lw $t0, 0($t0)");
    PUSH("$t0");
  }

  void HandleLoadConst(const ByteCode &code) {
    w.WriteLine("li $t0,", code.GetIntOperand());
    PUSH("$t0");
  }

  void HandleLoadString(const ByteCode &code) {
    w.WriteLine("la $t0,", context.GetStringLiteralLable(code.GetIntOperand()));
    PUSH("$t0");
  }

  void HandleStoreLocal(const ByteCode &code) {

  }

  void HandleStoreGlobal(const ByteCode &code) {

  }

  void HandleBinary(const char *op) {
    POP("$t0");
    POP("$t1");
    w.WriteLine(op, "$t2, $t0, $t1");
    PUSH("$t2");
  }

  void HandleBinaryAdd(const ByteCode &code) {
    HandleBinary("add");
  }

  void HandleBinarySub(const ByteCode &code) {
    HandleBinary("sub");
  }

  void HandleBinaryMultiply(const ByteCode &code) {
    HandleBinary("mul");
  }

  void HandleBinaryDivide(const ByteCode &code) {
    HandleBinary("div");
  }

  void HandleUnaryPositive(const ByteCode &code) {
    // Nop
  }

  void HandleUnaryNegative(const ByteCode &code) {
    w.WriteLine("lw $t0, 0($sp)");
    w.WriteLine("sub $t0, $zero, $t0");
    w.WriteLine("sw, $t0, 0($sp)");
  }

  void HandleCallFunction(const ByteCode &code) {
    auto label = context.GetGlobalLabel(code.GetStrOperand());
    w.WriteLine("jal", label);
    auto nargs = code.GetIntOperand();
    w.WriteLine("addi $sp,", context.GetStackOffset(nargs));
    PUSH("$v0");
  }

  void HandleReturnValue(const ByteCode &code) {
    POP("$v0");
    auto label = context.GetReturnLabel();
    w.WriteLine("j", label);
  }

  void HandleReturnNone(const ByteCode &code) {
    auto label = context.GetReturnLabel();
    w.WriteLine("j", label);
  }

  void HandlePrint(int syscall_code) {
    w.WriteLine("li $v0,", syscall_code);
    POP("$a0");
    w.WriteLine("syscall");
  }

  void HandlePrintString(const ByteCode &code) {
    /* print string */
    /* v0 = 4 */
    /* $a0 = address of null-terminated string to print */
    HandlePrint(4);
  }

  void HandlePrintCharacter(const ByteCode &code) {
    /* print character */
    /* 11 */
    /* $a0 = character to print */
    HandlePrint(11);
  }

  void HandlePrintInteger(const ByteCode &code) {
    /* print integer */
    /* 1 */
    /* $a0 = integer to print */
    HandlePrint(1);
  }

  void HandleRead(int syscall_code) {
    w.WriteLine("li $v0,", syscall_code);
    w.WriteLine("syscall");
    PUSH("$v0");
  }

  void HandleReadInteger(const ByteCode &code) {
    /* read integer */
    /* 5 */
    /* $v0 contains integer read */
    HandleRead(5);
  }

  void HandleReadCharacter(const ByteCode &code) {
    /* read character */
    /* 12 */
    /* $v0 contains character read */
    HandleRead(12);
  }

  void HandleBinarySubscr(const ByteCode &code) {

  }

  void HandleStoreSubscr(const ByteCode &code) {

  }

  void HandleUnaryJumpIf(const char *op, const ByteCode &code) {
    POP("$t0");
    auto label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine(op, "$t0, $zero,", label);
  }

  void HandleJumpIfTrue(const ByteCode &code) {
    HandleUnaryJumpIf("beq", code);
  }

  void HandleJumpIfFalse(const ByteCode &code) {
    HandleUnaryJumpIf("bne", code);
  }

  void HandleJumpForward(const ByteCode &code) {
    auto label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine("j", label);
  }

  void HandleBinaryJumpIf(const char *op, const ByteCode &code) {
    POP("$t0");
    POP("$t1");
    auto label = context.GetTargetLabel(code.GetIntOperand());
    w.WriteLine(op, "$t0, $t1,", label);
  }

  void HandleJumpIfNotEqual(const ByteCode &code) {
    HandleBinaryJumpIf("bne", code);
  }

  void HandleJumpIfEqual(const ByteCode &code) {
    HandleBinaryJumpIf("beq", code);
  }

  void HandleJumpIfGreater(const ByteCode &code) {
    HandleBinaryJumpIf("bge", code);
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

  void HandlePopTop(const ByteCode &code) {
    POP();
  }

};


class FunctionAssembler {
  const CompiledFunction &source;
  AssemblyWriter &sink;
  LocalContext context;

  void MakePrologue() {}
  void MakeEpilogue() {}

  String MakeTargetLabel(int offset) {
    return "";
  }

public:
  FunctionAssembler(const CompiledFunction &source, AssemblyWriter &sink):
    source(source), sink(sink) {}

  void Assemble() {
    MakePrologue();
    ByteCodeToMipsTranslator translator(sink, context);
    for (const auto &byteCode: source.GetCode()) {
      auto offset = byteCode.GetOffset();
      if (context.IsTarget(offset)) {
        sink.WriteLine(MakeTargetLabel(offset));
      }
      translator.dispatch(byteCode);
    }
    MakeEpilogue();
  }

};

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


};

// serve as a template translating one ByteCode to MIPS instructions
class ByteCodeToMipsTranslator: public OpcodeDispatcher<ByteCodeToMipsTranslator> {
  AssemblyWriter &w;
public:
  ByteCodeToMipsTranslator(AssemblyWriter &w): w(w) {}


  void HandleLoadLocal(const ByteCode &code) {

  }

  void HandleLoadGlobal(const ByteCode &code) {

  }

  void HandleLoadConst(const ByteCode &code) {

  }

  void HandleLoadString(const ByteCode &code) {

  }

  void HandleStoreLocal(const ByteCode &code) {

  }

  void HandleStoreGlobal(const ByteCode &code) {

  }

  void HandleBinaryAdd(const ByteCode &code) {

  }

  void HandleBinarySub(const ByteCode &code) {

  }

  void HandleBinaryMultiply(const ByteCode &code) {

  }

  void HandleBinaryDivide(const ByteCode &code) {

  }

  void HandleUnaryNot(const ByteCode &code) {

  }

  void HandleUnaryPositive(const ByteCode &code) {

  }

  void HandleUnaryNegative(const ByteCode &code) {

  }

  void HandleCallFunction(const ByteCode &code) {

  }

  void HandleReturnValue(const ByteCode &code) {

  }

  void HandleReturnNone(const ByteCode &code) {

  }

  void HandlePrintString(const ByteCode &code) {

  }

  void HandlePrintCharacter(const ByteCode &code) {

  }

  void HandlePrintInteger(const ByteCode &code) {

  }

  void HandleReadInteger(const ByteCode &code) {

  }

  void HandleReadCharacter(const ByteCode &code) {

  }

  void HandleBinarySubscr(const ByteCode &code) {

  }

  void HandleStoreSubscr(const ByteCode &code) {

  }

  void HandleJumpIfTrue(const ByteCode &code) {

  }

  void HandleJumpIfFalse(const ByteCode &code) {

  }

  void HandleJumpForward(const ByteCode &code) {

  }

  void HandleJumpIfNotEqual(const ByteCode &code) {

  }

  void HandleJumpIfEqual(const ByteCode &code) {

  }

  void HandleJumpIfGreater(const ByteCode &code) {

  }

  void HandleJumpIfGreaterEqual(const ByteCode &code) {

  }

  void HandleJumpIfLess(const ByteCode &code) {

  }

  void HandleJumpIfLessEqual(const ByteCode &code) {

  }

  void HandlePopTop(const ByteCode &code) {

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
    ByteCodeToMipsTranslator translator(sink);
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

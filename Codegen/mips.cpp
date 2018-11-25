#include "mips.h"
#include "OpcodeDispatcher.h"

class AssemblyWriter {
  std::ostream &os;
public:
  AssemblyWriter(std::ostream &os): os(os) {}

  template <typename... Args>
  void WriteLine(Args&&... args) {
    Print(os, std::forward<Args>(args)...);
  }

};

class ByteCodeToMipsTranslator: public OpcodeDispatcher<ByteCodeToMipsTranslator> {
  AssemblyWriter &w;
public:
  ByteCodeToMipsTranslator(AssemblyWriter &w): w(w) {}

  void HandleLoadLocal(const ByteCode &code) {

  }

};

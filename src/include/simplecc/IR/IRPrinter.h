#ifndef SIMPLECC_IR_IRPRINTER_H
#define SIMPLECC_IR_IRPRINTER_H
#include "simplecc/Support/IndentAwarePrinter.h"
#include "GlobalVariable.h"
#include "BasicBlock.h"

namespace simplecc {
class Module;
class Function;

class IRPrinter : IndentAwarePrinter<IRPrinter> {
public:
  void printModule(const Module &M);
  void printFunction(const Function &F);
  template<typename Iterator, typename PrintFn>
  void printValueList(Iterator first, Iterator last, PrintFn Print);
private:
  std::ostream &OS;
  friend IndentAwarePrinter;
  std::ostream &getOstream() {
    return OS;
  }
  void printNewline();
  void printGlobalVariable(const GlobalVariable &Var);
  void printBasicBlock(const simplecc::BasicBlock &BB);
};

template<typename Iterator, typename PrintFn>
void IRPrinter::printValueList(Iterator first, Iterator last, PrintFn Print) {
  for (; first != last; ++first) {
    Print(*first);
    if (first != last - 1)
      OS << "\n";
  }
}

}

#endif //SIMPLECC_IR_IRPRINTER_H
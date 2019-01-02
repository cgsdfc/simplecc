#ifndef SIMPLECC_IR_IRCOMPILER_H
#define SIMPLECC_IR_IRCOMPILER_H
#include "simplecc/Analysis/ContextualVisitor.h"
#include "simplecc/IR/IRBuilder.h"

namespace simplecc {
class Module;
class IRCompiler : ContextualVisitor<IRCompiler> {

  void visitFuncDef(FuncDef *FD);

public:
  IRCompiler() = default;
  bool Compile(Program *P, const SymbolTable &S, Module &M);
private:
  IRBuilder Builder;
};

}
#endif //SIMPLECC_IR_IRCOMPILER_H
#ifndef SIMPLECC_IR_IRCOMPILER_H
#define SIMPLECC_IR_IRCOMPILER_H
#include "simplecc/Analysis/ContextualVisitor.h"
#include "simplecc/IR/IRBuilder.h"

namespace simplecc {
class Module;
class IRCompiler : ContextualVisitor<IRCompiler> {

  void visitProgram(Program *P);
  void visitFuncDef(FuncDef *FD);
  void visitArgDecl(ArgDecl *AD);
  void visitVarDecl(VarDecl *VD);
  void visitConstDecl(ConstDecl *CD);


public:
  IRCompiler() = default;
  bool Compile(Program *P, const SymbolTable &S, Module &M);
private:
  IRBuilder Builder;
};

}
#endif //SIMPLECC_IR_IRCOMPILER_H
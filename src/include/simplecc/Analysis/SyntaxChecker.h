#ifndef SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#define SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
class SyntaxChecker : private VisitorBase<SyntaxChecker> {
  void visitProgram(Program *P);
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD);
  void visitArgDecl(ArgDecl *AD);
  void visitFuncDef(FuncDef *FD);

public:
  SyntaxChecker() : VisitorBase(), EM("SyntaxError") {}
  bool Check(Program *P);

private:
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif

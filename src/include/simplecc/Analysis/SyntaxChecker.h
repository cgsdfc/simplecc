#ifndef SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#define SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
class Program;

class SyntaxChecker : private VisitorBase<SyntaxChecker> {
  ErrorManager EM;
  friend VisitorBase;

  void visitProgram(Program *P);

  void visitConstDecl(ConstDecl *CD);

  void visitVarDecl(VarDecl *VD);

  void visitArgDecl(ArgDecl *AD);

  void visitFuncDef(FuncDef *FD);

public:
  SyntaxChecker() : VisitorBase(), EM("SyntaxError") {}
  bool Check(Program *P);
};
} // namespace simplecc
#endif

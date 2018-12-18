#ifndef SYNTAX_CHECKER_H
#define SYNTAX_CHECKER_H
#include "ErrorManager.h"
#include "Visitor.h"

namespace simplecc {
class Program;

class SyntaxChecker : private VisitorBase<SyntaxChecker> {
  ErrorManager EM;
  friend class VisitorBase<SyntaxChecker>;

  void visitProgram(Program *P);

  void visitConstDecl(ConstDecl *CD);

  void visitVarDecl(VarDecl *VD);

  void visitArgDecl(ArgDecl *AD);

  void visitFuncDef(FuncDef *FD);

public:
  SyntaxChecker() : VisitorBase(), EM("SyntaxError") {}
  bool Check(Program *P);

};

bool CheckSyntax(Program *P);
} // namespace simplecc
#endif

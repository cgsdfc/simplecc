#ifndef SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#define SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#include "simplecc/Analysis/Visitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// SyntaxChecker performs syntax checks on the program.
/// Since the Parser is based on a rewritten grammar, it loosens the restrictions
/// of the original one. Malformed code can be accepted by the Parser, so this class
/// rejects them.
/// The syntax checks includes:
/// 1. The order of global declarations should be: ``ConstDecl VarDecl FuncDef``.
/// 2. Size of Array cannot be zero.
/// 3. Variable types cannot be void.
/// 4. Type of a ConstDecl must match its value.
/// 5. The last declaration of a program must be the main function with a proper signature.
class SyntaxChecker : VisitorBase<SyntaxChecker> {
  void visitProgram(ProgramAST *P);
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD);
  void visitArgDecl(ArgDecl *AD);
  void visitFuncDef(FuncDef *FD);
  /// Return if a decl is void main().
  bool isMainFunction(DeclAST *D) const;

public:
  SyntaxChecker();
  /// Perform syntax check on the program.
  /// Return true is the program is ill-formed.
  bool Check(ProgramAST *P);

private:
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
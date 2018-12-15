#include "AstVerifier.h"
#include "ErrorManager.h"
#include "Visitor.h"

namespace simplecompiler {
class AstVerifier : ChildrenVisitor<AstVerifier> {

  /// Helper to check each condition in one single statement.
  void AssertThat(bool Pred, const char *ErrMsg) {
    if (Pred)
      return;
    EM.InternalError(ErrMsg);
  }

  void visitRead(Read *RD) {
    for (Expr *E : RD->getNames()) {
      AssertThat(IsInstance<Name>(E), "Names in Read must be Name");
    }
  }

  void visitWrite(Write *WR) {
    AssertThat(WR->getStr() || WR->getValue(),
               "Both Str and Value of Write are empty");
  }

  void visitAssign(Assign *A) {
    AssertThat(IsInstance<Name>(A->getTarget()) ||
                   IsInstance<Subscript>(A->getTarget()),
               "Target of Assign must be Name or Subscript");
  }

  void visitBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      AssertThat(IsInstance<BinOp>(B->getValue()), "HasCmpOp implies BinOp");
    }
  }

  void visitExprStmt(ExprStmt *ES) {
    AssertThat(IsInstance<Call>(ES->getValue()), "ExprStmt must have a Call");
  }

  void visitConstDecl(ConstDecl *CD) {
    auto Val = CD->getValue();
    AssertThat(IsInstance<Char>(Val) || IsInstance<Num>(Val),
        "Value of ConstDecl must be Num or Char");
  }

public:
  AstVerifier() = default;
  ~AstVerifier() = default;

  bool Verify(Program *P) {
    visitProgram(P);
    return EM.IsOk();
  }

private:
  friend class ChildrenVisitor<AstVerifier>;
  ErrorManager EM;
};

bool VerifyAST(Program *P) { return AstVerifier().Verify(P); }

} // namespace simplecompiler

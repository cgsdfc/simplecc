#include "simplecc/Visualize/DescriptionVisitor.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/ASTNode.h"

#include <sstream>

using namespace simplecc;

/// Return a descriptive string for AR.
std::string DescriptionVisitor::makeDescription(const ASTNode &AR) {
  return VisitorBase::visitAST<std::string>(AR.get());
}

std::string DescriptionVisitor::visitConstDecl(ConstDecl *CD) {
  std::ostringstream O;
  /// lambda to extract the numeric value of a Num or Char.
  auto MakeCV = [](Expr *E) {
    if (auto x = subclass_cast<CharExpr>(E))
      return x->getC();
    if (auto x = subclass_cast<NumExpr>(E))
      return x->getN();
    assert(false && "Unknown Expr class");
  };

  O << "const " << CStringFromBasicTypeKind(CD->getType()) << " "
    << CD->getName() << " = " << MakeCV(CD->getValue());
  return O.str();
}

std::string DescriptionVisitor::visitVarDecl(VarDecl *VD) {
  std::ostringstream O;
  O << CStringFromBasicTypeKind(VD->getType()) << " " << VD->getName();
  if (VD->getIsArray()) {
    O << "[" << VD->getSize() << "]";
  }
  return O.str();
}

std::string DescriptionVisitor::visitFuncDef(FuncDef *FD) {
  std::ostringstream O;
  Print(O, CStringFromBasicTypeKind(FD->getReturnType()), FD->getName());
  return O.str();
}

std::string DescriptionVisitor::visitArgDecl(ArgDecl *A) {
  std::ostringstream O;
  Print(O, CStringFromBasicTypeKind(A->getType()), A->getName());
  return O.str();
}

std::string DescriptionVisitor::visitNum(NumExpr *N) {
  std::ostringstream O;
  O << N->getN();
  return O.str();
}

std::string DescriptionVisitor::visitChar(CharExpr *C) {
  std::ostringstream O;
  O << "'" << static_cast<char>(C->getC()) << "'";
  return O.str();
}

std::string DescriptionVisitor::visitStr(StrExpr *S) {
  std::ostringstream O;
  O << S->getS();
  return O.str();
}
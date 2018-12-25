#include "simplecc/Visualize/DescriptionVisitor.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/AstRef.h"

#include <sstream>

using namespace simplecc;

/// Return a descriptive string for AR.
String DescriptionVisitor::makeDescription(const AstRef &AR) {
  switch (AR.getKind()) {
#define HANDLE_AST_TYPE(NAME)                                                  \
  case AstKind::NAME:                                                          \
    return visit##NAME(static_cast<NAME *>(AR.get()));
    HANDLE_AST_TYPE(Decl)
    HANDLE_AST_TYPE(Expr)
    HANDLE_AST_TYPE(Stmt)
#undef HANDLE_AST_TYPE
  default:
    return "";
  }
}

String DescriptionVisitor::visitConstDecl(ConstDecl *CD) {
  std::ostringstream O;
  /// lambda to extract the numeric value of a Num or Char.
  auto MakeCV = [](Expr *E) {
    if (auto x = subclass_cast<Char>(E))
      return x->getC();
    if (auto x = subclass_cast<Num>(E))
      return x->getN();
    assert(false && "Unknown Expr class");
  };

  O << "const " << CStringFromBasicTypeKind(CD->getType()) << " "
    << CD->getName() << " = " << MakeCV(CD->getValue());
  return O.str();
}

String DescriptionVisitor::visitVarDecl(VarDecl *VD) {
  std::ostringstream O;
  O << CStringFromBasicTypeKind(VD->getType()) << " " << VD->getName();
  if (VD->getIsArray()) {
    O << "[" << VD->getSize() << "]";
  }
  return O.str();
}

String DescriptionVisitor::visitFuncDef(FuncDef *FD) {
  std::ostringstream O;
  Print(O, CStringFromBasicTypeKind(FD->getReturnType()), FD->getName());
  return O.str();
}

String DescriptionVisitor::visitArgDecl(ArgDecl *A) {
  std::ostringstream O;
  Print(O, CStringFromBasicTypeKind(A->getType()), A->getName());
  return O.str();
}

String DescriptionVisitor::visitNum(Num *N) {
  std::ostringstream O;
  O << N->getN();
  return O.str();
}

String DescriptionVisitor::visitChar(Char *C) {
  std::ostringstream O;
  O << "'" << static_cast<char>(C->getC()) << "'";
  return O.str();
}

String DescriptionVisitor::visitStr(Str *S) {
  std::ostringstream O;
  O << S->getS();
  return O.str();
}
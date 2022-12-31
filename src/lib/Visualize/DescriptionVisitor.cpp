// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/Visualize/DescriptionVisitor.h"
#include "simplecc/Support/Print.h"
#include "simplecc/Visualize/ASTNode.h"
#include <sstream>

using namespace simplecc;

/// Return a descriptive string for AR.
std::string DescriptionVisitor::makeDescription(const ASTNode &AN) {
  return VisitorBase::visitAST<std::string>(AN.get());
}

std::string DescriptionVisitor::visitConstDecl(ConstDecl *CD) {
  std::ostringstream O;
  /// lambda to extract the numeric value of a Num or Char.
  auto MakeCV = [](ExprAST *E) {
    if (auto x = subclass_cast<CharExpr>(E))
      return x->getChar();
    if (auto x = subclass_cast<NumExpr>(E))
      return x->getNum();
    assert(false && "Unknown ExprAST class");
  };

  O << "const " << CStringFromBasicTypeKind(CD->getType()) << " "
    << CD->getName() << " = " << MakeCV(CD->getValue());
  return O.str();
}

std::string DescriptionVisitor::visitVarDecl(VarDecl *VD) {
  std::ostringstream O;
  O << CStringFromBasicTypeKind(VD->getType()) << " " << VD->getName();
  if (VD->isArray()) {
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
  O << N->getNum();
  return O.str();
}

std::string DescriptionVisitor::visitChar(CharExpr *C) {
  std::ostringstream O;
  O << "'" << static_cast<char>(C->getChar()) << "'";
  return O.str();
}

std::string DescriptionVisitor::visitStr(StrExpr *S) {
  std::ostringstream O;
  O << S->getStr();
  return O.str();
}
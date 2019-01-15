#include "simplecc/Analysis/Types.h"
#include "simplecc/Support/Casting.h"

using namespace simplecc;

ArrayType::ArrayType(const VarDecl *VD)
    : ElemType(VD->getType()), Size(VD->getSize()) {
  assert(VD->isArray());
}

ConstType::ConstType(const ConstDecl *CD) : Type(CD->getType()) {
  auto Val = CD->getValue();
  switch (Val->getKind()) {
  case ExprAST::CharExprKind:Value = static_cast<CharExpr *>(Val)->getChar();
    break;
  case ExprAST::NumExprKind:Value = static_cast<NumExpr *>(Val)->getNum();
    break;
  default:assert(false && "Value of ConstDecl must be a Constant!");
  }
}

VarType::VarType(const DeclAST *D) {
  switch (D->getKind()) {
  case DeclAST::ArgDeclKind:Type = static_cast<const ArgDecl *>(D)->getType();
    break;
  case DeclAST::VarDeclKind:Type = static_cast<const VarDecl *>(D)->getType();
    break;
  default:assert(false && "DeclAST not for Variable!");
  }
}

const char *SymbolEntry::getTypeName() const {
  if (IsFunction())
    return "Function";
  if (IsArray())
    return "Array";
  if (IsConstant())
    return "Constant";
  assert(IsVariable());
  return "Variable";
}

BasicTypeKind FuncType::getArgTypeAt(int Idx) const {
  assert(Idx < getNumArgs() && "Arg index out of range");
  return TheFuncDef->getArgAt(Idx)->getType();
}

VarType SymbolEntry::AsVariable() const {
  assert(IsVariable());
  return VarType(TheDecl);
}

FuncType SymbolEntry::AsFunction() const {
  assert(IsFunction());
  return FuncType(static_cast<const FuncDef *>(TheDecl));
}

ArrayType SymbolEntry::AsArray() const {
  assert(IsArray());
  return ArrayType(static_cast<const VarDecl *>(TheDecl));
}

ConstType SymbolEntry::AsConstant() const {
  assert(IsConstant());
  return ConstType(static_cast<const ConstDecl *>(TheDecl));
}

bool SymbolEntry::IsArray() const {
  return TheDecl && IsInstance<VarDecl>(TheDecl) &&
      static_cast<const VarDecl *>(TheDecl)->isArray();
}

bool SymbolEntry::IsVariable() const {
  return TheDecl && (IsInstance<ArgDecl>(TheDecl) ||
      (IsInstance<VarDecl>(TheDecl) &&
          !static_cast<const VarDecl *>(TheDecl)->isArray()));
}

bool SymbolEntry::IsConstant() const {
  return TheDecl && IsInstance<ConstDecl>(TheDecl);
}

bool SymbolEntry::IsFunction() const {
  return TheDecl && IsInstance<FuncDef>(TheDecl);
}

bool SymbolEntry::IsFormalArgument() const {
  return TheDecl && IsInstance<ArgDecl>(TheDecl);
}

Location SymbolEntry::getLocation() const {
  assert(TheDecl);
  return TheDecl->getLocation();
}

const std::string &SymbolEntry::getName() const {
  assert(TheDecl);
  return TheDecl->getName();
}

void SymbolEntry::Format(std::ostream &O) const {
  O << "SymbolEntry(" << getName() << ", " << getTypeName() << ", "
    << getScope() << ", " << getLocation() << ")";
}

SymbolEntry::SymbolEntry(Scope scope, const DeclAST *decl)
    : TheScope(scope), TheDecl(decl) {}

namespace simplecc {
std::ostream &operator<<(std::ostream &O, Scope S) {
  switch (S) {
  case Scope::Global:return O << "Global";
  case Scope::Local:return O << "Local";
  }
}
} // namespace simplecc

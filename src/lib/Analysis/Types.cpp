#include "simplecc/Analysis/Types.h"

using namespace simplecc;

ArrayType::ArrayType(VarDecl *VD)
    : ElemType(VD->getType()), Size(VD->getSize()) {
  assert(VD->getIsArray());
}

ConstType::ConstType(ConstDecl *CD) : Type(CD->getType()) {
  auto Val = CD->getValue();
  switch (Val->GetKind()) {
  case Expr::Char:
    Value = static_cast<Char *>(Val)->getC();
    break;
  case Expr::Num:
    Value = static_cast<Num *>(Val)->getN();
    break;
  default:
    assert(false && "Value of ConstDecl must be a Constant!");
  }
}

VarType::VarType(Decl *D) {
  switch (D->GetKind()) {
  case Decl::ArgDecl:
    Type = static_cast<ArgDecl *>(D)->getType();
    break;
  case Decl::VarDecl:
    Type = static_cast<VarDecl *>(D)->getType();
    break;
  default:
    assert(false && "Decl not for Variable!");
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

BasicTypeKind FuncType::getArgTypeAt(unsigned Idx) const {
  assert(Idx < getArgCount() && "Arg index out of range");
  auto Arg = TheFuncDef->getArgs()[Idx];
  return static_cast<ArgDecl *>(Arg)->getType();
}

VarType SymbolEntry::AsVariable() const {
  assert(IsVariable());
  return VarType(TheDecl);
}

FuncType SymbolEntry::AsFunction() const {
  assert(IsFunction());
  return FuncType(static_cast<FuncDef *>(TheDecl));
}

ArrayType SymbolEntry::AsArray() const {
  assert(IsArray());
  return ArrayType(static_cast<VarDecl *>(TheDecl));
}

ConstType SymbolEntry::AsConstant() const {
  assert(IsConstant());
  return ConstType(static_cast<ConstDecl *>(TheDecl));
}

bool SymbolEntry::IsArray() const {
  return TheDecl && IsInstance<VarDecl>(TheDecl) &&
         static_cast<VarDecl *>(TheDecl)->getIsArray();
}

bool SymbolEntry::IsVariable() const {
  return TheDecl && (IsInstance<ArgDecl>(TheDecl) ||
                  (IsInstance<VarDecl>(TheDecl) &&
                   !static_cast<VarDecl *>(TheDecl)->getIsArray()));
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

const Location &SymbolEntry::getLocation() const {
  assert(TheDecl);
  return TheDecl->getLoc();
}

const String &SymbolEntry::getName() const {
  assert(TheDecl);
  return TheDecl->getName();
}

void SymbolEntry::Format(std::ostream &O) const {
  O << "SymbolEntry(" << getName() << ", " << getTypeName() << ", "
    << getScope() << ", " << getLocation() << ")";
}

namespace simplecc {
std::ostream &operator<<(std::ostream &O, Scope S) {
  switch (S) {
  case Scope::Global:
    return O << "Global";
  case Scope::Local:
    return O << "Local";
  }
}
} // namespace simplecc

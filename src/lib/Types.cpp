#include "simplecc/Types.h"

using namespace simplecc;

ArrayType::ArrayType(VarDecl *VD)
    : ElemType(VD->getType()), Size(VD->getSize()) {
  assert(VD->getIsArray());
}

ConstType::ConstType(ConstDecl *CD) : type(CD->getType()) {
  auto Val = CD->getValue();
  switch (Val->GetKind()) {
  case Expr::Char:
    value = static_cast<Char *>(Val)->getC();
    break;
  case Expr::Num:
    value = static_cast<Num *>(Val)->getN();
    break;
  default:
    assert(false && "Value of ConstDecl must be a Constant!");
  }
}

VarType::VarType(Decl *D) {
  switch (D->GetKind()) {
  case Decl::ArgDecl:
    type = static_cast<ArgDecl *>(D)->getType();
    break;
  case Decl::VarDecl:
    type = static_cast<VarDecl *>(D)->getType();
    break;
  default:
    assert(false && "Decl not for Variable!");
  }
}

const char *SymbolEntry::GetTypeName() const {
  if (IsFunction())
    return "Function";
  if (IsArray())
    return "Array";
  if (IsConstant())
    return "Constant";
  assert(IsVariable());
  return "Variable";
}

BasicTypeKind FuncType::GetArgTypeAt(unsigned Idx) const {
  assert(Idx < GetArgCount());
  auto Arg = TheFuncDef->getArgs()[Idx];
  return static_cast<ArgDecl *>(Arg)->getType();
}

VarType SymbolEntry::AsVariable() const {
  assert(IsVariable());
  return VarType(decl);
}

FuncType SymbolEntry::AsFunction() const {
  assert(IsFunction());
  return FuncType(static_cast<FuncDef *>(decl));
}

ArrayType SymbolEntry::AsArray() const {
  assert(IsArray());
  return ArrayType(static_cast<VarDecl *>(decl));
}

ConstType SymbolEntry::AsConstant() const {
  assert(IsConstant());
  return ConstType(static_cast<ConstDecl *>(decl));
}

bool SymbolEntry::IsArray() const {
  return decl && IsInstance<VarDecl>(decl) &&
         static_cast<VarDecl *>(decl)->getIsArray();
}

bool SymbolEntry::IsVariable() const {
  return decl && (IsInstance<ArgDecl>(decl) ||
                  (IsInstance<VarDecl>(decl) &&
                   !static_cast<VarDecl *>(decl)->getIsArray()));
}

bool SymbolEntry::IsConstant() const {
  return decl && IsInstance<ConstDecl>(decl);
}

bool SymbolEntry::IsFunction() const {
  return decl && IsInstance<FuncDef>(decl);
}

bool SymbolEntry::IsFormalArgument() const {
  return decl && IsInstance<ArgDecl>(decl);
}

const Location &SymbolEntry::GetLocation() const {
  assert(decl);
  return decl->getLoc();
}

const String &SymbolEntry::GetName() const {
  assert(decl);
  return decl->getName();
}

void SymbolEntry::Format(std::ostream &O) const {
  O << "SymbolEntry(" << GetName() << ", " << GetTypeName() << ", "
    << GetScope() << ", " << GetLocation() << ")";
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

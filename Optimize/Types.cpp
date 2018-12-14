/// Types and SymbolEntry implementation.
#include "SymbolTable.h"

using namespace simplecompiler;

ConstType::ConstType(ConstDecl *decl) : type(decl->getType()) {
  if (auto x = subclass_cast<Char>(decl->getValue())) {
    value = x->getC();
  } else if (auto x = subclass_cast<Num>(decl->getValue())) {
    value = x->getN();
  } else {
    assert(false && "value of ConstDecl wrong type");
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

VarType SymbolEntry::AsVariable() const {
  assert(IsVariable());
  if (auto AD = subclass_cast<ArgDecl>(decl)) {
    return VarType(AD->getType());
  }
  return VarType(static_cast<VarDecl *>(decl)->getType());
}

BasicTypeKind FuncType::GetArgTypeAt(int pos) const {
  assert(pos >= 0 && pos < fun->args.size() && "pos out of range");
  return static_cast<ArgDecl *>(fun->args[pos])->getType();
}

bool SymbolEntry::IsFormalArgument() const { return IsInstance<ArgDecl>(decl); }

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
  return IsInstance<ArgDecl>(decl) ||
         (IsInstance<VarDecl>(decl) &&
          !static_cast<VarDecl *>(decl)->getIsArray());
}

bool SymbolEntry::IsConstant() const {
  return decl && IsInstance<ConstDecl>(decl);
}

bool SymbolEntry::IsFunction() const {
  return decl && IsInstance<FuncDef>(decl);
}

Location SymbolEntry::GetLocation() const { return decl->getLoc(); }

const String &SymbolEntry::GetName() const { return decl->getName(); }

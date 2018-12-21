#ifndef TYPES_H
#define TYPES_H
#include "simplecc/Parse/AST.h"
#include <cassert>

/// Types of simplecc
namespace simplecc {

enum class Scope { Global, Local };
std::ostream &operator<<(std::ostream &os, Scope s);

class FuncType {
  FuncDef *TheFuncDef;

public:
  explicit FuncType(FuncDef *FD) : TheFuncDef(FD) {}
  BasicTypeKind getReturnType() const { return TheFuncDef->getReturnType(); }
  BasicTypeKind getArgTypeAt(int Idx) const;
  size_t getArgCount() const { return TheFuncDef->getArgs().size(); }
};

class VarType {
  BasicTypeKind Type;

public:
  explicit VarType(BasicTypeKind Ty) : Type(Ty) {}
  explicit VarType(Decl *D);
  BasicTypeKind getType() const { return Type; }
};

class ArrayType {
  BasicTypeKind ElemType;
  unsigned Size;

public:
  explicit ArrayType(VarDecl *VD);
  BasicTypeKind getElementType() const { return ElemType; }
  unsigned getSize() const { return Size; }
};

class ConstType {
  int Value;
  BasicTypeKind Type;

public:
  explicit ConstType(ConstDecl *CD);
  int getValue() const { return Value; }
  BasicTypeKind getType() const { return Type; }
};

// An entry in the SymbolTable with type and scope information about
// a name within a block (global or local).
class SymbolEntry {
  Scope TheScope;
  Decl *TheDecl = nullptr;

public:
  SymbolEntry(Scope scope, Decl *decl) : TheScope(scope), TheDecl(decl) {}
  /// This constructs an invalid SymbolEntry.
  SymbolEntry() = default;

  bool IsFunction() const;
  FuncType AsFunction() const;

  bool IsArray() const;
  ArrayType AsArray() const;

  bool IsFormalArgument() const;
  bool IsVariable() const;
  VarType AsVariable() const;

  bool IsConstant() const;
  ConstType AsConstant() const;

  const char *getTypeName() const;

  const Location &getLocation() const;

  const String &getName() const;

  Scope getScope() const { return TheScope; }

  bool IsGlobal() const { return Scope::Global == getScope(); }
  bool IsLocal() const { return Scope::Local == getScope(); }

  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &os, const SymbolEntry &e) {
  e.Format(os);
  return os;
}

} // namespace simplecc
#endif

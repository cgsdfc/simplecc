#ifndef TYPES_H
#define TYPES_H
#include "simplecompiler/AST.h"
#include <cassert>

/// Types of simplecompiler
namespace simplecompiler {

enum class Scope { Global, Local };
std::ostream &operator<<(std::ostream &os, Scope s);

class FuncType {
  FuncDef *TheFuncDef;

public:
  explicit FuncType(FuncDef *FD) : TheFuncDef(FD) {}
  BasicTypeKind GetReturnType() const { return TheFuncDef->getReturnType(); }
  BasicTypeKind GetArgTypeAt(unsigned Idx) const;
  size_t GetArgCount() const { return TheFuncDef->getArgs().size(); }
};

class VarType {
  BasicTypeKind type;

public:
  explicit VarType(BasicTypeKind type) : type(type) {}
  explicit VarType(Decl *D);
  BasicTypeKind GetType() const { return type; }
};

class ArrayType {
  BasicTypeKind ElemType;
  unsigned Size;

public:
  explicit ArrayType(VarDecl *VD);
  BasicTypeKind GetElementType() const { return ElemType; }
  unsigned GetSize() const { return Size; }
};

class ConstType {
  int value;
  BasicTypeKind type;

public:
  explicit ConstType(ConstDecl *CD);
  int GetValue() const { return value; }
  BasicTypeKind GetType() const { return type; }
};

// An entry in the SymbolTable with type and scope information about
// a name within a block (global or local).
class SymbolEntry {
  Scope scope;
  Decl *decl = nullptr;

public:
  SymbolEntry(Scope scope, Decl *decl) : scope(scope), decl(decl) {}
  /// This constructs an invalid SymbolEntry.
  SymbolEntry() = default;

  bool IsFunction() const;
  FuncType AsFunction() const;

  bool IsArray() const;
  ArrayType AsArray() const;

  bool IsVariable() const;
  VarType AsVariable() const;

  bool IsConstant() const;
  ConstType AsConstant() const;

  const char *GetTypeName() const;

  const Location &GetLocation() const;

  const String &GetName() const;

  Scope GetScope() const { return scope; }

  bool IsGlobal() const { return Scope::Global == GetScope(); }
  bool IsLocal() const { return Scope::Local == GetScope(); }

  bool IsFormalArgument() const;

  void Format(std::ostream &os) const;
};

inline std::ostream &operator<<(std::ostream &os, const SymbolEntry &e) {
  e.Format(os);
  return os;
}

} // namespace simplecompiler
#endif

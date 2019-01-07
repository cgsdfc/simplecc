#ifndef SIMPLECC_ANALYSIS_TYPES_H
#define SIMPLECC_ANALYSIS_TYPES_H
#include "simplecc/Parse/AST.h"
#include "simplecc/Support/Macros.h"
#include <cassert>
#include <string>

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
  explicit VarType(DeclAST *D);
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
  DeclAST *TheDecl = nullptr;

public:
  SymbolEntry(Scope scope, DeclAST *decl) : TheScope(scope), TheDecl(decl) {}
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

  Location getLocation() const;

  const std::string &getName() const;

  Scope getScope() const { return TheScope; }

  bool IsGlobal() const { return Scope::Global == getScope(); }
  bool IsLocal() const { return Scope::Local == getScope(); }

  void Format(std::ostream &os) const;
};

DEFINE_INLINE_OUTPUT_OPERATOR(SymbolEntry)


} // namespace simplecc
#endif
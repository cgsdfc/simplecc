#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "AST.h"
#include <unordered_map>
#include <cstdint>

enum class Scope { Global, Local };

class FuncType {
  FuncDef *fun;
public:
  FuncType(FuncDef *fun): fun(fun) {}

  BasicTypeKind GetReturnType() const {
    return fun->return_type;
  }

  BasicTypeKind GetArgTypeAt(int pos) const {
    assert(pos >= 0 && pos < fun->args.size() && "pos out of range");
    return fun->args[pos]->type;
  }

  size_t GetArgCount() const {
    return fun->args.size();
  }

};

class VarType {
  BasicTypeKind type;
public:
  VarType(BasicTypeKind type): type(type) {}

  BasicTypeKind GetType() const {
    return type;
  }
};

class ArrayType {
  VarDecl *array;
public:
  ArrayType(VarDecl *array): array(array) {
    assert(array->is_array);
  }

  BasicTypeKind GetElementType() const {
    return array->type;
  }

  size_t GetSize() const {
    assert(array->size && "size of an array must > 0");
    return array->size;
  }

};

class ConstType {
  int value;
  BasicTypeKind type;
public:
  ConstType(ConstDecl *decl): type(decl->type) {
    if (auto x = subclass_cast<Char>(decl->value)) {
      value = x->c;
    }
    else if (auto x = subclass_cast<Num>(decl->value)) {
      value = x->n;
    }
    else {
      assert(false && "value of ConstDecl wrong type");
    }
  }

  int GetValue() const { return value; }
  BasicTypeKind GetType() const { return type; }
};

class SymbolEntry {
  Scope scope;
  Decl *decl;
  Arg *arg;
public:
  SymbolEntry(Scope scope, Decl *decl): scope(scope), decl(decl), arg(nullptr) {}
  SymbolEntry(Scope scope, Arg *arg): scope(scope), decl(nullptr), arg(arg) {}

  bool IsFunction() const {
    return decl && IsInstance<FuncDef>(decl);
  }

  bool IsArray() const {
    return decl && IsInstance<VarDecl>(decl) && static_cast<VarDecl *>(decl)->is_array;
  }

  bool IsVariable() const {
    return arg || (IsInstance<VarDecl>(decl) && !static_cast<VarDecl*>(decl)->is_array);
  }

  bool IsConstant() const {
    return decl && IsInstance<ConstDecl>(decl);
  }

  FuncType AsFunction() const {
    assert(IsFunction());
    return FuncType(static_cast<FuncDef*>(decl));
  }

  VarType AsVariable() const {
    assert(IsVariable());
    if (arg) {
      return VarType(arg->type);
    }
    assert(decl);
    return VarType(static_cast<VarDecl*>(decl)->type);
  }

  ArrayType AsArray() const {
    assert(IsArray());
    return ArrayType(static_cast<VarDecl*>(decl));
  }

  ConstType AsConstant() const {
    assert(IsConstant());
    return ConstType(static_cast<ConstDecl*>(decl));
  }

  const char *GetTypeName() const {
    if (IsFunction())
      return "Function";
    if (IsArray())
      return "Array";
    if (IsConstant())
      return "Constant";
    assert(IsVariable());
    return "Variable";
  }

  Location GetLocation() const {
    return decl ? decl->loc : arg->loc;
  }

  const String &GetName() const {
    return decl ? decl->name : arg->name;
  }

  Scope GetScope() const {
    return scope;
  }
};

using TableType = std::unordered_map<String, SymbolEntry>;
using NestedTableType = std::unordered_map<uintptr_t, TableType>;
using StringLiteralTable = std::unordered_map<String, int>;

class SymbolTableView {
  const TableType &subtable;
public:
  SymbolTableView(const TableType &subtable): subtable(subtable) {}
  // Provide a safe readonly access to value
  const SymbolEntry &operator[](const String &name) const {
    assert(subtable.count(name));
    return subtable.find(name)->second;
  }
};


class SymbolTable {
public:
  TableType global;
  NestedTableType locals;
  StringLiteralTable string_literals;

  SymbolTable(): global(), locals(), string_literals() {}

  SymbolTableView GetLocal(FuncDef *fun) const {
    auto key = reinterpret_cast<uintptr_t>(fun);
    assert(locals.count(key));
    return SymbolTableView(locals.find(key)->second);
  }

  SymbolTableView GetGlobal() const {
    return SymbolTableView(global);
  }

  int GetStringLiteralID(const String &literal) const {
    assert(string_literals.count(literal));
    return string_literals.find(literal)->second;
  }

  void Check() const;
};

std::ostream &operator<<(std::ostream &os, const SymbolTable &t);

bool BuildSymbolTable(Program *prog, SymbolTable &table);

#endif

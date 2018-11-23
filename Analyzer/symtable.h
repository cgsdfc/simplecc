#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "AST.h"
#include <unordered_map>

class Type {
public:
  int subclass_tag;

  Type(int subclass_tag): subclass_tag(subclass_tag) {}
  enum { Constant, Variable, Array, Function, };
  virtual ~Type() = 0;
  virtual Type *copy() const = 0;
  virtual const char *ClassName() const = 0;
};

inline Type::~Type() {}

class Constant: public Type {
public:
  BasicTypeKind type;

  Constant(BasicTypeKind type): Type(Type::Constant), type(type) {}
  ~Constant() {}
  Type *copy() const override { return new Constant(*this); }
  const char *ClassName() const override { return "Constant"; }
  static bool InstanceCheck(Type *x) {
    return x->subclass_tag == Type::Constant;
  }
};

class Variable: public Type {
public:
  BasicTypeKind type;

  Variable(BasicTypeKind type): Type(Type::Variable), type(type) {}
  ~Variable() override {}
  Type *copy() const override { return new Variable(*this); }
  const char *ClassName() const override { return "Variable"; }
  static bool InstanceCheck(Type *x) {
    return x->subclass_tag == Type::Variable;
  }
};

class Array: public Type {
public:
  BasicTypeKind elemtype;
  int size;

  Array(BasicTypeKind elemtype, int size): Type(Type::Array), elemtype(elemtype), size(size) {}
  ~Array() override {}
  Type *copy() const override { return new Array(*this); }
  const char *ClassName() const override { return "Array"; }
  static bool InstanceCheck(Type *x) {
    return x->subclass_tag == Type::Array;
  }
};

class Function: public Type {
public:
  BasicTypeKind return_type;
  std::vector<BasicTypeKind> args;

  Function(BasicTypeKind return_type, const std::vector<BasicTypeKind> &args):
    Type(Type::Function), return_type(return_type), args(args) {}
  ~Function() override {}
  Type *copy() const override { return new Function(*this); }
  const char *ClassName() const override { return "Function"; }
  static bool InstanceCheck(Type *x) {
    return x->subclass_tag == Type::Function;
  }
};

enum class Scope { Global, Local };

// Represent a single symbol
class Entry {
public:
  Type *type; // owned
  Scope scope;
  Location location;
  String name;

  Entry(): type(nullptr) {}
  Entry(Type *type, Scope scope, const Location &location, const String &name):
    type(type), scope(scope), location(location), name(name) {}

  // avoid double free
  Entry(Entry &&entry):
    Entry(entry.type, entry.scope, entry.location, entry.name) {
      entry.type = nullptr;
    }

  Entry(const Entry &entry):
    Entry(nullptr, entry.scope, entry.location, entry.name) {
      // type are owned, not shared
      type = entry.type->copy();
    }

  ~Entry() { delete type; }
};

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

  String GetName() const {
    return decl ? decl->name : arg->name;
  }

  Scope GetScope() const {
    return scope;
  }
};

using TableType = std::unordered_map<String, SymbolEntry>;
using NestedTableType = std::unordered_map<String, TableType>;

class SymbolTable {
public:
  TableType global;
  NestedTableType locals;

  SymbolTable(): global(), locals() {}

  const SymbolEntry &LookupLocal(const String &fun, const String &name) const;

  const SymbolEntry &LookupGlobal(const String &name) const;

  void Check() const;
};

// Provide access to each local namespace
class SymbolTableView {
  const SymbolTable &symtable;
  // point to the entry of the function being checked
  const SymbolEntry *cur_fun;
public:
  SymbolTableView(const SymbolTable &symtable):
    symtable(symtable), cur_fun(nullptr) {}

  const SymbolEntry &GetCurrentFunction() const {
    assert(cur_fun);
    return *cur_fun;
  }

  void SetCurrentFunction(FuncDef *fun) {
    cur_fun = &symtable.LookupGlobal(fun->name);
  }

  const SymbolEntry &Lookup(const String &name) {
    assert(cur_fun);
    return symtable.LookupLocal(cur_fun->GetName(), name);
  }

  const TableType &GetLocalTable() const {
    auto iter = symtable.locals.find(cur_fun->GetName());
    assert(iter != symtable.locals.end());
    return iter->second;
  }

  const TableType &GetGlobalTable() const {
    return symtable.global;
  }


};

std::ostream &operator<<(std::ostream &os, const SymbolTable &t);

bool BuildSymbolTable(Program *prog, SymbolTable &table);

#endif

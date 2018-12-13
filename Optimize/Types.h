#ifndef TYPES_H
#define TYPES_H
#include "AST.h"
#include <cassert>

/// Types of simplecompiler
namespace simplecompiler {

class FuncType {
  FuncDef *fun;

public:
  explicit FuncType(FuncDef *fun) : fun(fun) {}

  BasicTypeKind GetReturnType() const { return fun->return_type; }

  BasicTypeKind GetArgTypeAt(int pos) const;

  size_t GetArgCount() const { return fun->args.size(); }
};

class VarType {
  BasicTypeKind type;

public:
  explicit VarType(BasicTypeKind type) : type(type) {}

  BasicTypeKind GetType() const { return type; }
};

class ArrayType {
  VarDecl *array;

public:
  explicit ArrayType(VarDecl *array) : array(array) { assert(array->is_array); }

  BasicTypeKind GetElementType() const { return array->type; }

  size_t GetSize() const {
    assert(array->size && "size of an array must > 0");
    return array->size;
  }
};

class ConstType {
  int value;
  BasicTypeKind type;

public:
  explicit ConstType(ConstDecl *decl);
  int GetValue() const { return value; }
  BasicTypeKind GetType() const { return type; }
};

} // namespace simplecompiler
#endif

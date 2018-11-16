#include "symtable.h"

Type *DeclToType(ConstDecl *decl) {
  return new Constant(decl->type);
}

Type *DeclToType(VarDecl *decl) {
  auto  var_type = decl->type;
  if (var_type->is_array) {
    assert(var_type->size > 0);
    return new Array(var_type->type, var_type->size);
  }
  else {
    return new Variable(var_type->type);
  }
}

Type *DeclToType(FuncDef *decl) {
  auto fun = new Function(decl->return_type, {});
  for (auto arg: decl->args) {
    fun->args.push_back(arg->type);
  }
  return fun;
}

SymbolTable BuildSymbolTable(Program *node) {


}

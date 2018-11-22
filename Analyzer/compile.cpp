#include "compile.h"
#include "Visitor.h"

#include <unordered_map>

class Compiler;
class CompilerUnit;
class BasicBlock;
class ByteCode;

// Map hashable constants to its id
template <typename Value>
using OperandTable = std::unordered_map<Value, int>;

template <typename Value>
int MakeOperand(OperandTable<Value> &dict, const Value &val) {
  auto iter = dict.find(val);
  if (iter != dict.end()) {
    return val;
  }
  auto id = dict.size();
  dict.emplace(std::make_pair(val, id));
  return id;
}

// Transform all non-constant names in one table of specific scope into an OperandTable
// With an OperandTable one can translate names to their indices in names.
// constants are replaced with indices in consts.
void MakeNames(const TableType &table, Scope scope, OperandTable<String> &operands) {
  for (const auto &item : table) {
    const auto &entry = item.second;
    if (entry.scope == scope && !IsInstance<Constant>(entry.type)) {
      operands.emplace(std::make_pair(entry.name, operands.size()));
    }
  }
}

class BasicBlock;

class ByteCode {
public:
  Opcode opcode;
  int arg;
  BasicBlock *target;
  ByteCode(Opcode opcode, int arg = 0, BasicBlock *target = nullptr):
    opcode(opcode), arg(arg), target(target) {}
};

class BasicBlock {
public:
  BasicBlock *next;
  std::vector<ByteCode> instr;

  BasicBlock(): next(nullptr), instr() {}

  template <typename... Args>
  void AddByteCode(Args&&... args) {
    instr.emplace_back(std::forward<Args>(args)...);
  }
};

// Table holding various information found globally, like
// global integer constants, global string constants and global names
class GloablTable {
public:
  OperandTable<int> consts;
  OperandTable<String> strings;
  OperandTable<String> names;

  int GetConst(int val) {
    return MakeOperand(consts, val);
  }

  int GetString(const String &val) {
    return MakeOperand(strings, val);
  }

  int GetName(const String &name) {
    return MakeOperand(names, name);
  }


};

class Compiler {
public:
  // global const/literals tables
  CompilerUnit *unit;

  void visitProgam(Program *node) {


  }
};

using NamedConstTable = std::unordered_map<String, int>;

// "\"string\"" => "string"
String StripQuote(const String &s) {
  assert(s.size() >= 2);
  return s.substr(1, s.size() - 2);
}

class CompilerUnit: public VisitorBase<CompilerUnit> {
public:
  std::vector<BasicBlock*> blocks;
  BasicBlock *curblock;
  OperandTable<String> names;
  const TableType &symtable;
  GloablTable &global;
  // holds [name, val] mapping of ConstDecl
  NamedConstTable namedconsts;

  void AddByteCode(ByteCode c) {
    curblock->AddByteCode(c);
  }

  // val is the const value to LOAD_CONST
  void AddLoadConst(int val) {
    curblock->AddByteCode(Opcode::LOAD_CONST, global.GetConst(val));
  }

  BasicTypeKind visitExpr(Expr *node) {
    return VisitorBase::visitExpr<BasicTypeKind>(node);
  }

  void visitName(Name *node) {
    const auto &entry = symtable.at(node->id);
    if (IsInstance<Constant>(entry.type)) {
      // assert(node->ctx == ExprContextKind::Load);
      int val = namedconsts.at(entry.name);
      return AddLoadConst(val);
    }

    // assert(IsInstance<Array>(type) || IsInstance<Variable>(type));
    auto opcode = node->ctx == ExprContextKind::Load ? MakeLoad(entry.scope)
                                                     : MakeStore(entry.scope);
    auto arg = entry.scope == Scope::Global ? global.GetName(node->id)
                                            : MakeOperand(names, node->id);
    curblock->AddByteCode(opcode, arg);
  }

  BasicTypeKind visitChar(Char *node) {
    AddLoadConst(node->c);
    return BasicTypeKind::Character;
  }

  BasicTypeKind visitNum(Num *node) {
    AddLoadConst(node->n);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitStr(Str *node) {
    assert(false);
  }

  void visitWrite(Write *node) {
    if (node->str) {
      auto str = subclass_cast<Str>(node->str);
      assert(str);
      auto arg = global.GetString(StripQuote(str->s));
      curblock->AddByteCode(Opcode::PRINT_STRING, arg);
    }
    if (node->value) {
      auto type = visitExpr(node->value);
      assert(type != BasicTypeKind::Void);
      curblock->AddByteCode(MakePrint(type));
    }
  }

  // A Read operaton is translated to one READ_XXX followed by one
  // STORE_XXX
  void visitRead(Read *node) {
    for (auto expr: node->names) {
      auto name = subclass_cast<Name>(expr);
      const auto &entry = symtable.at(name->id);
      auto type = subclass_cast<Variable>(entry.type);
      curblock->AddByteCode(MakeRead(type->type));
      curblock->AddByteCode(MakeStore(entry.scope));
    }
  }

  BasicTypeKind visitParenExpr(ParenExpr *node) {
    visitExpr(node->value);
    return BasicTypeKind::Int;
  }

  void visitExprStmt(ExprStmt *node) {
    visitExpr(node->value);
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    visitExpr(node->operand);
    AddByteCode(MakeUnary(node->op));
    return BasicTypeKind::Int;
  }

  void visitReturn(Return *node) {
    if (node->value) {
      visitExpr(node->value);
      curblock->AddByteCode(Opcode::RETURN_VALUE);
    }
    else {
      curblock->AddByteCode(Opcode::RETURN_NONE);
    }
  }

  void visitFor(For *node) {


  }

  void visitWhile(While *node) {

  }

  void visitIf(If *node) {


  }

  void visitAssign(Assign *node) {


  }

  void visitSubscript(Subscript *node) {


  }
      
  void visitBoolOp(BoolOp *node) {


  }

  void visitCall(Call *node) 
};


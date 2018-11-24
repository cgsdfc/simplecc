#include "compile.h"
#include "Visitor.h"
#include "error.h"

#include <unordered_map>

using ByteCodeBuffer = std::vector<ByteCode>;
using NameTable = std::unordered_map<const char*, int>;

class FunctionCompiler: public VisitorBase<FunctionCompiler> {
  bool jump_negative;
  ByteCodeBuffer *buffer;
  SymbolTableView local;
  const SymbolTable &symtable;
  FuncDef *function;

  ByteCode &Add(ByteCode code) {
    buffer->push_back(code);
    return buffer->back();
  }

  ByteCode &GetLastByteCode() const {
    return buffer->back();
  }

  int GetOffset() const {
    return buffer->size();
  }

public:
  FunctionCompiler(FuncDef *fun, const SymbolTable &symtable):
    jump_negative(false),
    buffer(nullptr),
    local(symtable.GetLocal(fun)),
    symtable(symtable),
    function(fun) {}

  CompiledFunction *Compile() {
    const auto &entry = symtable.GetGlobal()[function->name];
    auto result = new CompiledFunction(local, entry);
    buffer = &result->code;
    for (auto s: function->stmts) {
      visitStmt(s);
    }
    return result;
  }

  void visitExpr(Expr *node) {
    VisitorBase::visitExpr<void>(node);
  }

  void visitStmt(Stmt *node) {
    VisitorBase::visitStmt<void>(node);
  }

  void visitStmtList(const std::vector<Stmt*> &statements) {
    for (auto s: statements) {
      visitStmt(s);
    }
  }

  void visitRead(Read *node) {
    for (auto expr: node->names) {
      auto name = static_cast<Name*>(expr);
      const auto &entry = local[name->id];
      Add(ByteCode(MakeRead(entry.AsVariable().GetType())));
      Add(ByteCode(MakeStore(entry.GetScope()), entry.GetName().data()));
    }
  }

  void visitWrite(Write *node) {
    if (node->str) {
      visitExpr(node->str);
      Add(ByteCode(Opcode::PRINT_STRING));
    }
    if (node->value) {
      visitExpr(node->value);
      auto opcode = IsInstance<Char>(node->value) ? Opcode::PRINT_CHARACTER :
                                                    Opcode::PRINT_INTEGER;
      Add(ByteCode(opcode));
    }
  }

  void visitBoolOp(BoolOp *node) {
    if (auto x = subclass_cast<BinOp>(node->value)) {
      visitExpr(x->left);
      visitExpr(x->right);
      Add(ByteCode(jump_negative ? MakeJumpNegative(x->op) : MakeJump(x->op)));
      return;
    }
    visitExpr(node->value);
    Add(ByteCode(jump_negative ? Opcode::JUMP_IF_FALSE : Opcode::JUMP_IF_TRUE));
  }

  void visitAssign(Assign *node) {
    visitExpr(node->value);
    visitExpr(node->target);
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    auto offset = GetOffset();
    for (auto s: node->body) {
      visitStmt(s);
    }
    visitStmt(node->step);
    jump_negative = false;
    visitExpr(node->condition);
  }

  void visitWhile(While *node) {
    auto offset = GetOffset();
    jump_negative = true;
    visitExpr(node->condition);
    auto &jump = GetLastByteCode();
    for (auto s: node->body) {
      visitStmt(s);
    }
    Add(ByteCode(Opcode::JUMP_FORWARD, offset));
    jump.SetTarget(GetOffset());
  }

  void visitIf(If *node) {
    jump_negative = true;
    visitExpr(node->test);
    auto &jump1 = GetLastByteCode();
    for (auto s: node->body) {
      visitStmt(s);
    }
    auto &jump2 = Add(ByteCode(Opcode::JUMP_FORWARD));
    for (auto s: node->orelse) {
      visitStmt(s);
    }
    auto target2 = GetOffset();
    jump2.SetTarget(target2);
    auto target1 = GetOffset();
    jump1.SetTarget(target1);
  }

  void visitReturn(Return *node) {
    if (node->value) {
      visitExpr(node->value);
    }
    else {
      Add(ByteCode(Opcode::LOAD_CONST, 0));
    }
    Add(ByteCode(Opcode::RETURN_VALUE));
  }

  void visitExprStmt(ExprStmt *node) {
    visitExpr(node->value);
    // discard return value
    Add(ByteCode(Opcode::POP_TOP));
  }

  void visitUnaryOp(UnaryOp *node) {
    visitExpr(node->operand);
    Add(ByteCode(MakeUnary(node->op)));
  }

  void visitBinOp(BinOp *node) {
    visitExpr(node->left);
    visitExpr(node->right);
    Add(ByteCode(MakeBinary(node->op)));
  }

  void visitParenExpr(ParenExpr *node) {
    visitExpr(node->value);
  }

  void visitCall(Call *node) {
    for (auto expr: node->args) {
      visitExpr(expr);
    }
    Add(ByteCode(
          Opcode::CALL_FUNCTION,
          node->args.size(),
          node->func.data()));
  }

  void visitNum(Num *node) {
    Add(ByteCode(Opcode::LOAD_CONST, node->n));
  }

  void visitStr(Str *node) {
    Add(ByteCode(Opcode::LOAD_STRING, symtable.GetStringLiteralID(node->s)));
  }

  void visitChar(Char *node) {
    Add(ByteCode(Opcode::LOAD_CONST, node->c));
  }

  void visitSubscript(Subscript *node) {
    const auto &entry = local[node->name];
    auto load = MakeLoad(entry.GetScope());
    Add(ByteCode(load, node->name.data()));
    visitExpr(node->index);
    Add(ByteCode(MakeSubScr(node->ctx)));
  }

  void visitName(Name *node) {
    const auto &entry = local[node->id];
    if (entry.IsConstant()) {
      Add(ByteCode(Opcode::LOAD_CONST, entry.AsConstant().GetValue()));
    }
    else {
      auto opcode = node->ctx == ExprContextKind::Load ? MakeLoad(entry.GetScope())
                                                       : MakeStore(entry.GetScope());
      Add(ByteCode(opcode, node->id.data()));
    }
  }

};

class ModuleCompiler {
  const SymbolTable &symtable;
  Program *program;

public:
  ModuleCompiler(Program *prog, const SymbolTable &symtable):
    program(prog), symtable(symtable) {}

  CompiledModule *Compile() {
    auto module = new CompiledModule(symtable.GetGlobal());
    for (auto decl: program->decls) {
      if (auto fun = subclass_cast<FuncDef>(decl)) {
        FunctionCompiler functionCompiler(fun, symtable);
        module->functions.push_back(functionCompiler.Compile());
      }
    }
    return module;
  }

};

CompiledModule *Compile(Program *prog, const SymbolTable &symtable) {
  ModuleCompiler moduleCompiler(prog, symtable);
  return moduleCompiler.Compile();
}

void CompiledFunction::Format(std::ostream &os) const {
  os << "CompiledFunction(" << Quote(GetName()) << "):\n";
  auto lineno = 1;
  for (const auto &code: GetCode()) {
    os << "\t" << lineno << ": " << code << "\n";
    lineno++;
  }
}

void CompiledModule::Format(std::ostream &os) const {
  for (auto fun: GetFunctions()) {
    os << fun << "\n";
  }
}

CompiledModule::~CompiledModule() {
  for (auto fun: GetFunctions()) {
    delete fun;
  }
}

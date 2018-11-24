#include "compile.h"
#include "Visitor.h"
#include "error.h"

#include <iomanip>
#include <unordered_map>

using ByteCodeBuffer = std::vector<ByteCode>;
using NameTable = std::unordered_map<const char*, int>;

class FunctionCompiler: public VisitorBase<FunctionCompiler> {
  unsigned current_lineno;
  bool jump_negative;
  ByteCodeBuffer *buffer;
  SymbolTableView local;
  const SymbolTable &symtable;
  FuncDef *function;

  ByteCode &Add(ByteCode code) {
    code.SetLineno(current_lineno);
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
    current_lineno(1),
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
    if (buffer->empty() || GetLastByteCode().GetOpcode() != Opcode::RETURN_VALUE) {
      // A missing reutrn_stmt, insert one.
      Add(ByteCode(Opcode::RETURN_NONE));
    }
    return result;
  }

  BasicTypeKind visitExpr(Expr *node) {
    current_lineno = node->loc.lineno;
    return VisitorBase::visitExpr<BasicTypeKind>(node);
  }

  void visitStmt(Stmt *node) {
    current_lineno = node->loc.lineno;
    VisitorBase::visitStmt<void>(node);
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
      auto type = visitExpr(node->value);
      Add(ByteCode(MakePrint(type)));
    }
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
      Add(ByteCode(Opcode::RETURN_VALUE));
    }
    else {
      Add(ByteCode(Opcode::RETURN_NONE));
    }
  }

  void visitExprStmt(ExprStmt *node) {
    auto type = visitExpr(node->value);
    // discard return value
    Add(ByteCode(Opcode::POP_TOP));
  }

  BasicTypeKind visitBoolOp(BoolOp *node) {
    if (auto x = subclass_cast<BinOp>(node->value)) {
      visitExpr(x->left);
      visitExpr(x->right);
      Add(ByteCode(jump_negative ? MakeJumpNegative(x->op) : MakeJump(x->op)));
    }
    else {
      visitExpr(node->value);
      Add(ByteCode(jump_negative ? Opcode::JUMP_IF_FALSE : Opcode::JUMP_IF_TRUE));
    }
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitUnaryOp(UnaryOp *node) {
    visitExpr(node->operand);
    Add(ByteCode(MakeUnary(node->op)));
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitBinOp(BinOp *node) {
    visitExpr(node->left);
    visitExpr(node->right);
    Add(ByteCode(MakeBinary(node->op)));
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitParenExpr(ParenExpr *node) {
    visitExpr(node->value);
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitCall(Call *node) {
    for (auto expr: node->args) {
      visitExpr(expr);
    }
    Add(ByteCode(
          Opcode::CALL_FUNCTION,
          node->args.size(),
          node->func.data()));
    return local[node->func].AsFunction().GetReturnType();
  }

  BasicTypeKind visitNum(Num *node) {
    Add(ByteCode(Opcode::LOAD_CONST, node->n));
    return BasicTypeKind::Int;
  }

  BasicTypeKind visitStr(Str *node) {
    Add(ByteCode(Opcode::LOAD_STRING, symtable.GetStringLiteralID(node->s)));
    return BasicTypeKind::Void;
  }

  BasicTypeKind visitChar(Char *node) {
    Add(ByteCode(Opcode::LOAD_CONST, node->c));
    return BasicTypeKind::Character;
  }

  BasicTypeKind visitSubscript(Subscript *node) {
    const auto &entry = local[node->name];
    auto load = MakeLoad(entry.GetScope());
    Add(ByteCode(load, node->name.data()));
    visitExpr(node->index);
    Add(ByteCode(MakeSubScr(node->ctx)));
    return entry.AsArray().GetElementType();
  }

  BasicTypeKind visitName(Name *node) {
    const auto &entry = local[node->id];
    if (entry.IsConstant()) {
      Add(ByteCode(Opcode::LOAD_CONST, entry.AsConstant().GetValue()));
      return entry.AsConstant().GetType();
    }
    else {
      auto opcode = node->ctx == ExprContextKind::Load ? MakeLoad(entry.GetScope())
                                                       : MakeStore(entry.GetScope());
      Add(ByteCode(opcode, node->id.data()));
      return entry.AsVariable().GetType();
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

CompiledModule *CompileProgram(Program *prog, const SymbolTable &symtable) {
  ModuleCompiler moduleCompiler(prog, symtable);
  return moduleCompiler.Compile();
}

void CompiledFunction::Format(std::ostream &os) const {
  os << "CompiledFunction(" << Quote(GetName()) << "):\n";
  auto lineno = 1;
  for (const auto &code: GetCode()) {
    os << std::setw(4) << lineno << ": " << code << "\n";
    lineno++;
  }
}

void CompiledModule::Format(std::ostream &os) const {
  for (auto fun: GetFunctions()) {
    os << *fun << "\n";
  }
}

CompiledModule::~CompiledModule() {
  for (auto fun: GetFunctions()) {
    delete fun;
  }
}

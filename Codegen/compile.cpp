#include "compile.h"
#include "Visitor.h"
#include "error.h"

#include <iomanip>
#include <unordered_map>

using ByteCodeBuffer = std::vector<ByteCode>;
using NameTable = std::unordered_map<const char *, int>;

class FunctionCompiler : public VisitorBase<FunctionCompiler> {
  unsigned current_lineno;
  ByteCodeBuffer buffer;
  SymbolTableView local;
  const SymbolTable &symtable;
  FuncDef *function;
  std::vector<SymbolEntry> formal_arguments;
  std::vector<SymbolEntry> local_objects;

  // add one piece of ByteCode to buffer, return the the offset
  // of it in buffer
  int Add(ByteCode code) {
    code.SetLineno(current_lineno);
    auto offset = GetNextByteCodeOffset();
    code.SetOffset(offset);
    buffer.push_back(code);
    return offset;
  }

  // return the offset of the next ByteCode to be added
  int GetNextByteCodeOffset() const { return buffer.size(); }

  // return the offset of the last added ByteCode
  int GetLastByteCodeOffset() const {
    assert(!buffer.empty() && "there should be at least one ByteCode");
    return buffer.size() - 1;
  }

  ByteCode GetLastByteCode() const { return buffer[GetLastByteCodeOffset()]; }

  void SetTargetAt(int offset, int target) {
    buffer.at(offset).SetTarget(target);
  }

public:
  FunctionCompiler(FuncDef *fun, const SymbolTable &symtable)
      : current_lineno(1), buffer(), local(symtable.GetLocal(fun)),
        symtable(symtable), function(fun) {}

  void visitFuncDef(FuncDef *node) {
    for (auto arg : node->args) {
      visitArg(arg);
    }
    for (auto decl : node->decls) {
      visitDecl(decl);
    }
    for (auto s : node->stmts) {
      visitStmt(s);
    }
  }

  CompiledFunction Compile() {
    visitFuncDef(function);
    if (buffer.empty() ||
        GetLastByteCode().GetOpcode() != Opcode::RETURN_VALUE) {
      // A missing reutrn_stmt, insert one.
      Add(ByteCode(Opcode::RETURN_NONE));
    }
    const auto &entry = symtable.GetGlobal()[function->name];
    return CompiledFunction(local, std::move(buffer), entry,
                            std::move(formal_arguments),
                            std::move(local_objects));
  }

  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }

  BasicTypeKind visitExpr(Expr *node) {
    current_lineno = node->loc.lineno;
    return VisitorBase::visitExpr<BasicTypeKind>(node);
  }

  void visitStmt(Stmt *node) {
    current_lineno = node->loc.lineno;
    VisitorBase::visitStmt<void>(node);
  }

  void visitArg(Arg *node) { formal_arguments.push_back(local[node->name]); }

  void visitVarDecl(VarDecl *node) {
    local_objects.push_back(local[node->name]);
  }

  void visitConstDecl(ConstDecl *) {}

  void visitRead(Read *node) {
    for (auto expr : node->names) {
      auto name = static_cast<Name *>(expr);
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
    Add(ByteCode(Opcode::PRINT_NEWLINE));
  }

  void visitAssign(Assign *node) {
    visitExpr(node->value);
    visitExpr(node->target);
  }

  int CompileBoolOp(BoolOp *node, bool is_negative) {
    if (auto binop = subclass_cast<BinOp>(node->value)) {
      // only BinOp introduce relation operator
      visitExpr(binop->left);
      visitExpr(binop->right);
      auto opcode =
          is_negative ? MakeJumpNegative(binop->op) : MakeJump(binop->op);
      return Add(ByteCode(opcode));
    }
    visitExpr(node->value);
    auto opcode = is_negative ? Opcode::JUMP_IF_FALSE : Opcode::JUMP_IF_TRUE;
    return Add(ByteCode(opcode));
  }

  void visitFor(For *node) {
    visitStmt(node->initial);
    auto offset = GetNextByteCodeOffset();
    for (auto s : node->body) {
      visitStmt(s);
    }
    visitStmt(node->step);
    auto jump_if = CompileBoolOp(static_cast<BoolOp *>(node->condition), false);
    SetTargetAt(jump_if, offset);
  }

  void visitWhile(While *node) {
    auto offset = GetNextByteCodeOffset();
    auto jump_if = CompileBoolOp(static_cast<BoolOp *>(node->condition), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    Add(ByteCode(Opcode::JUMP_FORWARD, offset));
    SetTargetAt(jump_if, GetNextByteCodeOffset());
  }

  void visitIf(If *node) {
    auto jump_if = CompileBoolOp(static_cast<BoolOp *>(node->test), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    if (node->orelse.empty()) {
      SetTargetAt(jump_if, GetNextByteCodeOffset());
      return;
    }
    auto jump_forward = Add(ByteCode(Opcode::JUMP_FORWARD));
    SetTargetAt(jump_if, GetNextByteCodeOffset());
    for (auto s : node->orelse) {
      visitStmt(s);
    }
    SetTargetAt(jump_forward, GetNextByteCodeOffset());
  }

  void visitReturn(Return *node) {
    if (node->value) {
      visitExpr(node->value);
      Add(ByteCode(Opcode::RETURN_VALUE));
    } else {
      Add(ByteCode(Opcode::RETURN_NONE));
    }
  }

  void visitExprStmt(ExprStmt *node) {
    auto type = visitExpr(node->value);
    // discard return value
    Add(ByteCode(Opcode::POP_TOP));
  }

  BasicTypeKind visitBoolOp(BoolOp *node) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
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
    for (auto expr : node->args) {
      visitExpr(expr);
    }
    Add(ByteCode(Opcode::CALL_FUNCTION, node->args.size(), node->func.data()));
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
    } else {
      auto opcode = node->ctx == ExprContextKind::Load
                        ? MakeLoad(entry.GetScope())
                        : MakeStore(entry.GetScope());
      Add(ByteCode(opcode, node->id.data()));
      return entry.AsVariable().GetType();
    }
  }
};

CompiledModule CompileProgram(Program *prog, const SymbolTable &symtable) {
  ObjectList global_objects;
  std::vector<CompiledFunction> functions;
  auto &&global = symtable.GetGlobal();

  for (auto decl : prog->decls) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      FunctionCompiler functionCompiler(fun, symtable);
      functions.push_back(functionCompiler.Compile());
    } else if (auto var = subclass_cast<VarDecl>(decl)) {
      global_objects.push_back(global[var->name]);
    }
  }
  return CompiledModule(std::move(functions), symtable.string_literals,
                        std::move(global_objects));
}

void CompiledFunction::Format(std::ostream &os) const {
  os << "CompiledFunction(" << Quote(GetName()) << "):\n";
  os << "formal_arguments:\n";
  for (const auto &arg : formal_arguments) {
    os << arg << "\n";
  }
  os << "\nlocal_objects:\n";
  for (const auto &obj : local_objects) {
    os << obj << "\n";
  }
  os << "\ncode:\n";

  auto lineno = 0;
  for (const auto &code : GetCode()) {
    os << std::setw(4) << lineno << ": " << code << "\n";
    lineno++;
  }
}

void CompiledModule::Format(std::ostream &os) const {
  os << "global_objects:\n";
  for (const auto &obj : global_objects) {
    os << obj << "\n";
  }
  os << "\nfunctions:\n";
  for (const auto &fun : functions) {
    os << fun << "\n";
  }
}

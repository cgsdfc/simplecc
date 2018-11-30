#include "compile.h"
#include "Visitor.h"
#include "error.h"

#include <iomanip>
#include <unordered_map>

namespace {
using namespace simplecompiler;
class FunctionCompiler : public VisitorBase<FunctionCompiler> {
  unsigned current_lineno;
  std::vector<ByteCode> buffer;
  SymbolTableView local;
  const SymbolTable &symtable;
  FuncDef *function;
  std::vector<SymbolEntry> formal_arguments;
  std::vector<SymbolEntry> local_objects;

  // add one piece of ByteCode to buffer, return the the offset
  // of it in buffer
  int Add(ByteCode code) {
    code.SetLineno(current_lineno);
    auto offset = GetLabel();
    code.SetOffset(offset);
    buffer.push_back(code);
    return offset;
  }

  // return the offset of the next ByteCode to be added
  int GetLabel() const { return buffer.size(); }

  // return the last ByteCode in the buffer
  ByteCode GetLastByteCode() const { return buffer.back(); }

  // set the jump ByteCode at offset to have target
  void SetTargetAt(int offset, int target) {
    assert(IsJumpXXX(buffer.at(offset).GetOpcode()) && "IsJumpXXX()");
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

  // public interface
  CompiledFunction Compile() {
    visitFuncDef(function);
    // XXX: this is wrong. the last instruction in the buffer is **not**
    // the last instruction of the last **basic block**.
    // until the real basic block implementation comes out, no test code
    // should reveal this bug -- they must end each function that returns
    // with a explicit outter most return
    if (buffer.empty() ||
        (GetLastByteCode().GetOpcode() != Opcode::RETURN_VALUE &&
         GetLastByteCode().GetOpcode() != Opcode::RETURN_NONE)) {
      // A missing reutrn_stmt, insert one.
      Add(ByteCode(Opcode::RETURN_NONE));
    }
    const auto &entry = symtable.GetGlobal(function->name);
    return CompiledFunction(local, std::move(buffer), entry,
                            std::move(formal_arguments),
                            std::move(local_objects));
  }

  void visitDecl(Decl *node) { VisitorBase::visitDecl<void>(node); }

  void visitExpr(Expr *node) {
    current_lineno = node->loc.lineno;
    VisitorBase::visitExpr<void>(node);
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
      visitExpr(node->value);
      auto type = symtable.GetExprType(node->value).GetType();
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
    auto jump_to_start = Add(ByteCode(Opcode::JUMP_FORWARD));
    auto loop_label = GetLabel();
    visitStmt(node->step);
    auto jump_to_end =
        CompileBoolOp(static_cast<BoolOp *>(node->condition), true);
    auto start_label = GetLabel();
    for (auto s : node->body) {
      visitStmt(s);
    }
    auto jump_to_loop = Add(ByteCode(Opcode::JUMP_FORWARD));
    auto end_label = GetLabel();

    SetTargetAt(jump_to_start, start_label);
    SetTargetAt(jump_to_end, end_label);
    SetTargetAt(jump_to_loop, loop_label);
  }

  void visitWhile(While *node) {
    auto loop_label = GetLabel();
    auto jump_to_end =
        CompileBoolOp(static_cast<BoolOp *>(node->condition), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    auto jump_to_loop = Add(ByteCode(Opcode::JUMP_FORWARD));
    auto end_label = GetLabel();

    SetTargetAt(jump_to_end, end_label);
    SetTargetAt(jump_to_loop, loop_label);
  }

  void visitIf(If *node) {
    auto jump_to_orelse =
        CompileBoolOp(static_cast<BoolOp *>(node->test), true);
    for (auto s : node->body) {
      visitStmt(s);
    }
    if (node->orelse.empty()) {
      auto end_label = GetLabel();
      SetTargetAt(jump_to_orelse, end_label);
      return;
    }
    auto jump_to_end = Add(ByteCode(Opcode::JUMP_FORWARD));
    auto orelse_label = GetLabel();
    for (auto s : node->orelse) {
      visitStmt(s);
    }
    auto end_label = GetLabel();

    SetTargetAt(jump_to_orelse, orelse_label);
    SetTargetAt(jump_to_end, end_label);
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
    visitExpr(node->value);
    // discard return value
    Add(ByteCode(Opcode::POP_TOP));
  }

  void visitBoolOp(BoolOp *node) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
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

  void visitParenExpr(ParenExpr *node) { visitExpr(node->value); }

  void visitCall(Call *node) {
    for (auto expr : node->args) {
      visitExpr(expr);
    }
    Add(ByteCode(Opcode::CALL_FUNCTION, node->args.size(), node->func.data()));
  }

  void visitNum(Num *node) { Add(ByteCode(Opcode::LOAD_CONST, node->n)); }

  void visitStr(Str *node) {
    Add(ByteCode(Opcode::LOAD_STRING, symtable.GetStringLiteralID(node->s)));
  }

  void visitChar(Char *node) { Add(ByteCode(Opcode::LOAD_CONST, node->c)); }

  void visitSubscript(Subscript *node) {
    const auto &entry = local[node->name];
    auto load = MakeLoad(entry.GetScope());
    // load array
    Add(ByteCode(load, node->name.data()));
    // calculate index
    visitExpr(node->index);
    // do subscript
    Add(ByteCode(MakeSubScr(node->ctx)));
  }

  void visitName(Name *node) {
    const auto &entry = local[node->id];
    if (entry.IsConstant()) {
      Add(ByteCode(Opcode::LOAD_CONST, entry.AsConstant().GetValue()));
    } else {
      // 4 choices: (LOAD, STORE) * (GLOBAL, LOCAL)
      auto opcode = node->ctx == ExprContextKind::Load
                        ? MakeLoad(entry.GetScope())
                        : MakeStore(entry.GetScope());
      Add(ByteCode(opcode, node->id.data()));
    }
  }
};
} // namespace

CompiledModule simplecompiler::CompileProgram(Program *prog,
                                              const SymbolTable &symtable) {
  ObjectList global_objects;
  std::vector<CompiledFunction> functions;

  for (auto decl : prog->decls) {
    if (auto fun = subclass_cast<FuncDef>(decl)) {
      FunctionCompiler functionCompiler(fun, symtable);
      functions.push_back(functionCompiler.Compile());
    } else if (auto var = subclass_cast<VarDecl>(decl)) {
      global_objects.push_back(symtable.GetGlobal(var->name));
    }
  }
  return CompiledModule(std::move(functions), symtable.GetStringLiteralTable(),
                        std::move(global_objects));
}

CompiledFunction::CompiledFunction(SymbolTableView local,
                                   std::vector<ByteCode> &&code,
                                   SymbolEntry entry,
                                   ObjectList &&formal_arguments,
                                   ObjectList &&local_objects)
    : local(local), code(std::move(code)), entry(entry),
      formal_arguments(std::move(formal_arguments)),
      local_objects(std::move(local_objects)) {
  assert(entry.IsFunction());
  for (auto &&code : code) {
    code.Check();
  }
}

CompiledFunction::CompiledFunction(CompiledFunction &&other)
    : local(other.local), code(std::move(other.code)), entry(other.entry),
      formal_arguments(std::move(other.formal_arguments)),
      local_objects(std::move(other.local_objects)) {}

void CompiledFunction::Format(std::ostream &os) const {
  os << "CompiledFunction(" << Quote(GetName()) << "):\n";
  os << "\nformal_arguments:\n";
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
  os << "\nstring_literals:\n";
  for (auto &&item : strings) {
    os << std::setw(4) << item.second << ": " << item.first << "\n";
  }
}

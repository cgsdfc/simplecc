#include "Compile.h"
#include "ErrorManager.h"
#include "Visitor.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace simplecompiler {

class ByteCodeBuilder {

  static Opcode MakeSubScr(ExprContextKind ctx) {
    switch (ctx) {
    case ExprContextKind::Load:
      return Opcode::BINARY_SUBSCR;
    case ExprContextKind::Store:
      return Opcode::STORE_SUBSCR;
    }
  }

  static Opcode MakeLoad(Scope scope) {
    switch (scope) {
    case Scope::Global:
      return Opcode::LOAD_GLOBAL;
    case Scope::Local:
      return Opcode::LOAD_LOCAL;
    }
  }

  static Opcode MakeStore(Scope scope) {
    switch (scope) {
    case Scope::Global:
      return Opcode::STORE_GLOBAL;
    case Scope::Local:
      return Opcode::STORE_LOCAL;
    }
  }

  static Opcode MakeRead(BasicTypeKind type) {
    switch (type) {
    case BasicTypeKind::Character:
      return Opcode::READ_CHARACTER;
    case BasicTypeKind::Int:
      return Opcode::READ_INTEGER;
    default:
      assert(false);
    }
  }

  static Opcode MakePrint(BasicTypeKind type) {
    switch (type) {
    case BasicTypeKind::Character:
      return Opcode::PRINT_CHARACTER;
    case BasicTypeKind::Int:
      return Opcode::PRINT_INTEGER;
    default:
      assert(false);
    }
  }

  static Opcode MakeBinary(OperatorKind oper) {
    switch (oper) {
    case OperatorKind::Add:
      return Opcode::BINARY_ADD;
    case OperatorKind::Sub:
      return Opcode::BINARY_SUB;
    case OperatorKind::Mult:
      return Opcode::BINARY_MULTIPLY;
    case OperatorKind::Div:
      return Opcode::BINARY_DIVIDE;
    default:
      assert(false);
    }
  }

  static Opcode MakeUnary(UnaryopKind oper) {
    switch (oper) {
    case UnaryopKind::UAdd:
      return Opcode::UNARY_POSITIVE;
    case UnaryopKind::USub:
      return Opcode::UNARY_NEGATIVE;
    }
  }

  static Opcode MakeJumpNegative(OperatorKind oper) {
    switch (oper) {
    case OperatorKind::NotEq:
      return Opcode::JUMP_IF_EQUAL;
    case OperatorKind::Eq:
      return Opcode::JUMP_IF_NOT_EQUAL;
    case OperatorKind::GtE:
      return Opcode::JUMP_IF_LESS;
    case OperatorKind::Gt:
      return Opcode::JUMP_IF_LESS_EQUAL;
    case OperatorKind::LtE:
      return Opcode::JUMP_IF_GREATER;
    case OperatorKind::Lt:
      return Opcode::JUMP_IF_GREATER_EQUAL;
    default:
      assert(false);
    }
  }

  static Opcode MakeJump(OperatorKind oper) {
    switch (oper) {
    case OperatorKind::Eq:
      return Opcode::JUMP_IF_EQUAL;
    case OperatorKind::NotEq:
      return Opcode::JUMP_IF_NOT_EQUAL;
    case OperatorKind::Lt:
      return Opcode::JUMP_IF_LESS;
    case OperatorKind::LtE:
      return Opcode::JUMP_IF_LESS_EQUAL;
    case OperatorKind::Gt:
      return Opcode::JUMP_IF_GREATER;
    case OperatorKind::GtE:
      return Opcode::JUMP_IF_GREATER_EQUAL;
    default:
      assert(false);
    }
  }

  /// Insert a ByteCode into the back of InsertPoint.
  /// Return the offset of the inserted ByteCode.
  unsigned Insert(ByteCode Code) {
    CompiledFunction &TheFunction = *getInsertPoint();
    auto Off = TheFunction.size();
    TheFunction.GetByteCodeList().push_back(std::move(Code));
    return Off;
  }

  unsigned CreateNullary(Opcode Op) { return Insert(ByteCode(Op)); }

public:
  ByteCodeBuilder() = default;
  ~ByteCodeBuilder() = default;

  void setInsertPoint(CompiledFunction *F) { InsertPoint = F; }
  CompiledFunction *getInsertPoint() const { return InsertPoint; }

  void setLocation(const Location &L) { CurrentLineno = L.getLineNo(); }

  unsigned CreateRead(BasicTypeKind T) { return Insert(MakeRead(T)); }

  unsigned CreatePrint(BasicTypeKind T) { return Insert(MakePrint(T)); }

  unsigned CreatePrintNewline() { return CreateNullary(Opcode::PRINT_NEWLINE); }

  unsigned CreatePrintString() { return CreateNullary(Opcode::PRINT_STRING); }

  unsigned CreateJumpIfFalse() { return CreateNullary(Opcode::JUMP_IF_FALSE); }

  unsigned CreateJump(OperatorKind CompareOp, bool IsNeg = true) {
    return CreateNullary(IsNeg ? MakeJumpNegative(CompareOp)
                               : MakeJump(CompareOp));
  }

  unsigned CreateJumpForward() { return CreateNullary(Opcode::JUMP_FORWARD); }

  unsigned CreateBinary(OperatorKind Op) {
    return CreateNullary(MakeBinary(Op));
  }

  unsigned CreateReturnValue() { return CreateNullary(Opcode::RETURN_VALUE); }

  unsigned CreateReturnNone() { return CreateNullary(Opcode::RETURN_NONE); }

  unsigned CreatePopTop() { return CreateNullary(Opcode::POP_TOP); }

  unsigned CreateUnary(UnaryopKind Op) { return CreateNullary(MakeUnary(Op)); }

  unsigned CreateCallFunction(const String &Name, unsigned Argc) {
    return Insert(ByteCode(Opcode::CALL_FUNCTION, Argc, Name.data()));
  }

  unsigned CreateLoad(Scope S, const String &Name) {
    return Insert(ByteCode(MakeLoad(S), Name.data()));
  }

  unsigned CreateStore(Scope S, const String &Name) {
    return Insert(ByteCode(MakeStore(S), Name.data()));
  }

  unsigned CreateLoadConst(int ConstValue) {
    return Insert(ByteCode(Opcode::LOAD_CONST, ConstValue));
  }

  unsigned CreateLoadString(unsigned StringID) {
    return Insert(ByteCode(Opcode::LOAD_STRING, StringID));
  }

  unsigned CreateSubscr(ExprContextKind Context) {
    return CreateNullary(MakeSubScr(Context));
  }

  void setJumpTargetAt(unsigned Idx, unsigned Target) {
    ByteCode &TheCode = getInsertPoint()->getByteCodeAt(Idx);
    assert(IsJumpXXX(TheCode.GetOpcode()) &&
           "Attempt to set jump target for non-jump ByteCode!");
    TheCode.SetTarget(Target);
  }

  /// Return the size of the current CompiledFunction.
  unsigned getSize() const { return getInsertPoint()->size(); }

private:
  CompiledFunction *InsertPoint = nullptr;
  unsigned CurrentLineno = 1;
};

class ByteCodeCompiler : ChildrenVisitor<ByteCodeCompiler> {

  void visitStmt(Stmt *S) {
    Builder.setLocation(S->getLoc());
    ChildrenVisitor::visitStmt(S);
  }

  void visitArgDecl(ArgDecl *A) {
    auto TheFunction = Builder.getInsertPoint();
    TheFunction->GetFormalArguments().push_back(TheLocalTable[A->getName()]);
  }

  void visitVarDecl(VarDecl *VD) {
    auto TheFunction = Builder.getInsertPoint();
    TheFunction->GetLocalObjects().push_back(TheLocalTable[VD->getName()]);
  }

  /// Explicitly do nothing.
  void visitConstDecl(ConstDecl *) {}

  void visitRead(Read *RD) {
    for (auto E : RD->getNames()) {
      auto N = static_cast<Name *>(E);
      const auto &Entry = TheLocalTable[N->getId()];
      Builder.CreateRead(Entry.AsVariable().GetType());
      Builder.CreateStore(Entry.GetScope(), Entry.GetName());
    }
  }

  void visitWrite(Write *WR) {
    if (WR->getStr()) {
      visitExpr(WR->getStr());
      Builder.CreatePrintString();
    }
    if (WR->getValue()) {
      visitExpr(WR->getValue());
      Builder.CreatePrint(TheTable->getExprType(WR->getValue()));
    }
    Builder.CreatePrintNewline();
  }

  unsigned CompileBoolOp(BoolOp *B) {
    ChildrenVisitor::visitBoolOp(B);
    if (B->getHasCmpop()) {
      return Builder.CreateJump(static_cast<BinOp *>(B->getValue())->getOp());
    }
    return Builder.CreateJumpIfFalse();
  }

  void visitFor(For *node) {
    visitStmt(node->getInitial());
    unsigned JumpToStart = Builder.CreateJumpForward();

    unsigned Loop = Builder.getSize();
    visitStmt(node->getStep());
    unsigned JumpToEnd =
        CompileBoolOp(static_cast<BoolOp *>(node->getCondition()));

    unsigned Start = Builder.getSize();
    for (auto s : node->getBody()) {
      visitStmt(s);
    }
    unsigned JumpToLoop = Builder.CreateJumpForward();
    unsigned End = Builder.getSize();

    Builder.setJumpTargetAt(JumpToStart, Start);
    Builder.setJumpTargetAt(JumpToEnd, End);
    Builder.setJumpTargetAt(JumpToLoop, Loop);
  }

  void visitWhile(While *W) {
    unsigned Loop = Builder.getSize();
    unsigned JumpToEnd =
        CompileBoolOp(static_cast<BoolOp *>(W->getCondition()));

    for (auto s : W->getBody()) {
      visitStmt(s);
    }
    unsigned JumpToLoop = Builder.CreateJumpForward();

    unsigned End = Builder.getSize();
    Builder.setJumpTargetAt(JumpToEnd, End);
    Builder.setJumpTargetAt(JumpToLoop, Loop);
  }

  void visitIf(If *I) {
    unsigned JumpToElse = CompileBoolOp(static_cast<BoolOp *>(I->getTest()));
    for (auto s : I->getBody()) {
      visitStmt(s);
    }

    if (I->getOrelse().empty()) {
      unsigned End = Builder.getSize();
      Builder.setJumpTargetAt(JumpToElse, End);
      return;
    }
    unsigned JumpToEnd = Builder.CreateJumpForward();

    unsigned Else = Builder.getSize();
    for (auto s : I->getOrelse()) {
      visitStmt(s);
    }
    unsigned End = Builder.getSize();

    Builder.setJumpTargetAt(JumpToElse, Else);
    Builder.setJumpTargetAt(JumpToEnd, End);
  }

  void visitReturn(Return *R) {
    ChildrenVisitor::visitReturn(R);
    R->getValue() ? Builder.CreateReturnValue() : Builder.CreateReturnNone();
  }

  void visitExprStmt(ExprStmt *ES) {
    ChildrenVisitor::visitExprStmt(ES);
    // discard return value
    Builder.CreatePopTop();
  }

  void visitBoolOp(BoolOp *) {
    assert(false && "BoolOp should be handled by CompileBoolOp()");
  }

  void visitUnaryOp(UnaryOp *U) {
    ChildrenVisitor::visitUnaryOp(U);
    Builder.CreateUnary(U->getOp());
  }

  void visitBinOp(BinOp *B) {
    ChildrenVisitor::visitBinOp(B);
    Builder.CreateBinary(B->getOp());
  }

  void visitCall(Call *C) {
    /// Visit all the args first.
    ChildrenVisitor::visitCall(C);
    Builder.CreateCallFunction(C->getFunc(), C->getArgs().size());
  }

  void visitNum(Num *N) { Builder.CreateLoadConst(N->getN()); }

  void visitStr(Str *S) {
    Builder.CreateLoadString(TheModule->GetStringLiteralID(S->getS()));
  }

  void visitChar(Char *C) { Builder.CreateLoadConst(C->getC()); }

  void visitSubscript(Subscript *SB) {
    const auto &Entry = TheLocalTable[SB->getName()];
    // load array
    Builder.CreateLoad(Entry.GetScope(), SB->getName());
    // calculate index
    visitExpr(SB->getIndex());
    // do subscript
    Builder.CreateSubscr(SB->getCtx());
  }

  void visitName(Name *N) {
    const auto &Entry = TheLocalTable[N->getId()];
    if (Entry.IsConstant()) {
      Builder.CreateLoadConst(Entry.AsConstant().GetValue());
      return;
    }
    N->getCtx() == ExprContextKind::Load
        ? Builder.CreateLoad(Entry.GetScope(), N->getId())
        : Builder.CreateStore(Entry.GetScope(), N->getId());
  }

  void visitFuncDef(FuncDef *FD) {
    auto TheFunction = CompiledFunction::Create(TheModule);
    TheFunction->setName(FD->getName());
    Builder.setInsertPoint(TheFunction);
    setLocalTable(TheTable->getLocalTable(FD));
    ChildrenVisitor::visitFuncDef(FD);
  }

  void setModule(CompiledModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(SymbolTableView V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;

  // public interface
  void Compile(Program *P, const SymbolTable &S, CompiledModule &Module) {
    EM.clear();
    setTable(&S);
    setModule(&Module);
    visitProgram(P);
  }

private:
  friend class ChildrenVisitor<ByteCodeCompiler>;
  friend class VisitorBase<ByteCodeCompiler>;

  ByteCodeBuilder Builder;
  SymbolTableView TheLocalTable;
  const SymbolTable *TheTable;
  CompiledModule *TheModule;
  ErrorManager EM;
};

CompiledFunction::CompiledFunction(CompiledModule *M) : Parent(M) {
  /// Owned by Module
  M->getFunctionList().push_back(this);
}

void CompiledModule::Build(Program *P, const SymbolTable &S) {
  ByteCodeCompiler().Compile(P, S, *this);
}

unsigned CompiledModule::GetStringLiteralID(const String &Str) {
  auto ID = StringLiterals.size();
  return StringLiterals.emplace(Str, ID).first->second;
}

void CompiledFunction::Format(std::ostream &os) const {
  os << "CompiledFunction(" << Quote(getName()) << "):\n";
  os << "\nformal_arguments:\n";
  for (const auto &arg : GetFormalArguments()) {
    os << arg << "\n";
  }
  os << "\nlocal_objects:\n";
  for (const auto &obj : GetLocalObjects()) {
    os << obj << "\n";
  }
  os << "\ncode:\n";

  auto lineno = 0;
  for (const auto &code : *this) {
    os << std::setw(4) << lineno << ": " << code << "\n";
    lineno++;
  }
}

void CompiledModule::Format(std::ostream &os) const {
  os << "global_objects:\n";
  for (const auto &obj : GetGlobalObjects()) {
    os << obj << "\n";
  }
  os << "\nfunctions:\n";
  for (const auto &fun : *this) {
    os << fun << "\n";
  }
  os << "\nstring_literals:\n";
  for (auto &&item : GetStringLiteralTable()) {
    os << std::setw(4) << item.second << ": " << item.first << "\n";
  }
}

} // namespace simplecompiler

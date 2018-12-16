#include "Compile.h"
#include "ErrorManager.h"
#include "Visitor.h"

#include <algorithm>
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

  static Opcode MakeBinary(OperatorKind Op) {
    switch (Op) {
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

  static Opcode MakeUnary(UnaryopKind Op) {
    switch (Op) {
    case UnaryopKind::UAdd:
      return Opcode::UNARY_POSITIVE;
    case UnaryopKind::USub:
      return Opcode::UNARY_NEGATIVE;
    }
  }

  static Opcode MakeJumpNegative(OperatorKind Op) {
    switch (Op) {
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

  static Opcode MakeJump(OperatorKind Op) {
    switch (Op) {
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
  //
  unsigned Insert(ByteCode Code) {
    /// get the function being built.
    ByteCodeFunction &TheFunction = *getInsertPoint();

    /// Fill in other members of Code.
    auto Off = TheFunction.size();
    Code.SetSourceLineno(getLineNo());
    Code.SetByteCodeOffset(Off);

    /// Insert Code at the back of the function.
    TheFunction.GetByteCodeList().push_back(std::move(Code));
    return Off;
  }

  /// Create a ByteCode and insert it into the InsertPoint.
  /// This essentially forwards arguments to ByteCode::Create().
  template <typename... Args> unsigned Create(Opcode Op, Args &&... ExtraArgs) {
    return Insert(ByteCode::Create(Op, std::forward<Args>(ExtraArgs)...));
  }

public:
  ByteCodeBuilder() = default;
  ~ByteCodeBuilder() = default;

  unsigned CreateRead(BasicTypeKind T) { return Create(MakeRead(T)); }
  unsigned CreatePrint(BasicTypeKind T) { return Create(MakePrint(T)); }
  unsigned CreatePrintNewline() { return Create(Opcode::PRINT_NEWLINE); }
  unsigned CreatePrintString() { return Create(Opcode::PRINT_STRING); }
  unsigned CreateJumpIfFalse() { return Create(Opcode::JUMP_IF_FALSE); }
  unsigned CreateJumpIfTrue() { return Create(Opcode::JUMP_IF_TRUE); }

  unsigned CreateJump(OperatorKind CompareOp, bool IsNeg = true) {
    return Create(IsNeg ? MakeJumpNegative(CompareOp) : MakeJump(CompareOp));
  }

  unsigned CreateJumpForward() { return Create(Opcode::JUMP_FORWARD); }
  unsigned CreateBinary(OperatorKind Op) { return Create(MakeBinary(Op)); }
  unsigned CreateReturnValue() { return Create(Opcode::RETURN_VALUE); }
  unsigned CreateReturnNone() { return Create(Opcode::RETURN_NONE); }
  unsigned CreatePopTop() { return Create(Opcode::POP_TOP); }
  unsigned CreateUnary(UnaryopKind Op) { return Create(MakeUnary(Op)); }

  unsigned CreateCallFunction(const String &Name, unsigned Argc) {
    return Create(Opcode::CALL_FUNCTION, Name.data(), Argc);
  }

  unsigned CreateLoad(Scope S, const String &Name) {
    return Create(MakeLoad(S), Name.data());
  }

  unsigned CreateStore(Scope S, const String &Name) {
    return Create(MakeStore(S), Name.data());
  }

  unsigned CreateLoadConst(int ConstValue) {
    return Create(Opcode::LOAD_CONST, ConstValue);
  }

  unsigned CreateLoadString(unsigned StringID) {
    return Create(Opcode::LOAD_STRING, StringID);
  }

  unsigned CreateSubscr(ExprContextKind Context) {
    return Create(MakeSubScr(Context));
  }

  void setJumpTargetAt(unsigned Idx, unsigned Target) {
    ByteCode &TheCode = getInsertPoint()->getByteCodeAt(Idx);
    TheCode.SetJumpTarget(Target);
  }

  void setInsertPoint(ByteCodeFunction *F) { InsertPoint = F; }

  ByteCodeFunction *getInsertPoint() const {
    assert(InsertPoint && "InsertPoint not set!");
    return InsertPoint;
  }

  void setLocation(const Location &L) { CurrentLineno = L.getLineNo(); }
  unsigned getLineNo() const { return CurrentLineno; }

  /// Return the size of the current ByteCodeFunction.
  unsigned getSize() const { return getInsertPoint()->size(); }

private:
  ByteCodeFunction *InsertPoint = nullptr;
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
    TheFunction->GetLocalVariables().push_back(TheLocalTable[VD->getName()]);
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

  /// Visit value first and then target. The order of ChildrenVisitor::visitAssign
  /// is unfortunately wrong.
  void visitAssign(Assign *A) {
    visitExpr(A->getValue());
    visitExpr(A->getTarget());
  }

  unsigned CompileBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      BinOp *BO = static_cast<BinOp*>(B->getValue());
      visitExpr(BO->getLeft());
      visitExpr(BO->getRight());
      return Builder.CreateJump(BO->getOp());
    }

    ChildrenVisitor::visitBoolOp(B);
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
  void visitChar(Char *C) { Builder.CreateLoadConst(C->getC()); }
  void visitStr(Str *S) {
    Builder.CreateLoadString(TheModule->GetStringLiteralID(S->getS()));
  }

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
    /// Create the function and set members.
    auto TheFunction = ByteCodeFunction::Create(TheModule);
    TheFunction->setName(FD->getName());
    setLocalTable(TheTable->getLocalTable(FD));
    TheFunction->setLocalTable(TheLocalTable);

    /// Emit code for the function body.
    Builder.setInsertPoint(TheFunction);
    ChildrenVisitor::visitFuncDef(FD);

    // It is important we always return since we don't
    // construct any BasicBlock to detect inproper returns.
    Builder.CreateReturnNone();
  }

  void visitProgram(Program *P) {
    for (Decl *D : P->getDecls()) {
      switch (D->GetKind()) {
      case Decl::FuncDef:
        visitFuncDef(static_cast<FuncDef *>(D));
        break;
      case Decl::VarDecl:
        // Collect global objects.
        TheModule->GetGlobalVariables().push_back(
            TheTable->getGlobalEntry(D->getName()));
        break;
      default:
        break; // Ignore ConstDecl.
      }
    }
  }

  void setModule(ByteCodeModule *M) { TheModule = M; }
  void setTable(const SymbolTable *S) { TheTable = S; }
  void setLocalTable(SymbolTableView V) { TheLocalTable = V; }

public:
  ByteCodeCompiler() = default;

  // public interface
  void Compile(Program *P, const SymbolTable &S, ByteCodeModule &Module) {
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
  const SymbolTable *TheTable = nullptr;
  ByteCodeModule *TheModule = nullptr;
  ErrorManager EM;
};

ByteCodeFunction::ByteCodeFunction(ByteCodeModule *M) : Parent(M) {
  /// Owned by Module
  M->getFunctionList().push_back(this);
}

ByteCodeModule::~ByteCodeModule() {
  /// Delete all owned functions.
  std::for_each(begin(), end(), [](ByteCodeFunction *F) { delete F; });
}

void ByteCodeModule::Build(Program *P, const SymbolTable &S) {
  clear();
  ByteCodeCompiler().Compile(P, S, *this);
}

unsigned ByteCodeModule::GetStringLiteralID(const String &Str) {
  auto ID = StringLiterals.size();
  return StringLiterals.emplace(Str, ID).first->second;
}

void ByteCodeModule::clear() {
  FunctionList.clear();
  StringLiterals.clear();
  GlobalVariables.clear();
}

void ByteCodeFunction::Format(std::ostream &O) const {
  O << getName() << ":\n";

  for (const auto &Arg : GetFormalArguments()) {
    O << Arg << "\n";
  }

  for (const auto &LV : GetLocalVariables()) {
    O << LV << "\n";
  }

  for (const auto &Code : *this) {
    O << Code << "\n";
  }
}

void ByteCodeModule::Format(std::ostream &O) const {
  for (const auto &GV : GetGlobalVariables()) {
    O << GV << "\n";
  }

  O << "\n";
  for (auto &&Pair : GetStringLiteralTable()) {
    O << std::setw(4) << Pair.second << ": " << Pair.first << "\n";
  }

  O << "\n";
  for (const auto &Fn : *this) {
    O << *Fn << "\n";
  }

}

} // namespace simplecompiler

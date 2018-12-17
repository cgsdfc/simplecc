#include "simplecompiler/ByteCodeModule.h"
#include "simplecompiler/ByteCodeBuilder.h"
#include "simplecompiler/ByteCodeFunction.h"
#include "simplecompiler/ErrorManager.h"
#include "simplecompiler/Visitor.h"

#include <cassert>
#include <utility>

namespace simplecompiler {

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

  /// Visit value first and then target. The order of
  /// ChildrenVisitor::visitAssign is unfortunately wrong.
  void visitAssign(Assign *A) {
    visitExpr(A->getValue());
    visitExpr(A->getTarget());
  }

  unsigned CompileBoolOp(BoolOp *B) {
    if (B->getHasCmpop()) {
      BinOp *BO = static_cast<BinOp *>(B->getValue());
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

void ByteCodeModule::Build(Program *P, const SymbolTable &S) {
  clear();
  ByteCodeCompiler().Compile(P, S, *this);
}
} // namespace simplecompiler

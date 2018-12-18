//
// Created by cgsdfc on 18-12-18.
//

#ifndef SIMPLECOMPILER_LLVMIRCOMPILER_H
#define SIMPLECOMPILER_LLVMIRCOMPILER_H
#include "simplecc/Visitor.h"
#include <llvm/ADT/StringRef.h>

namespace llvm {
class Value;
class Function;
}

namespace simplecc {
using llvm::Value;
using llvm::Function;

/// This class emits LLVM IR from an AST.
///
/// The principle it enforces is:
/// 1. Don't change the semantic of the user's program.
/// 2. Be well-formed in terms of LLVM IR.
/// 3. Avoid useless stuffs.
class LLVMIRCompiler : VisitorBase<LLVMIRCompiler> {
  bool visitStmtList(const std::vector<Stmt *> &StatementList);
  Value *visitUnaryOp(UnaryOp *U);
  /// Declare builtin functions (external really, but let's call it builtin).
  void DeclareBuiltinFunctions();
  Function *DeclareIOBuiltins(const char *Name);
  Value *getString(llvm::StringRef Str);

  Value *visitExpr(Expr *E) { return VisitorBase::visitExpr<Value *>(E); }
  void visitArgDecl(ArgDecl *) {}

};
}

#endif //SIMPLECOMPILER_LLVMIRCOMPILER_H

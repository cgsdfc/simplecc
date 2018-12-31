#ifndef SIMPLECC_LLVM_LLVMVALUEMAP_H
#define SIMPLECC_LLVM_LLVMVALUEMAP_H
#include "simplecc/LLVM/LLVMTypeMap.h"
#include <llvm/IR/Constants.h>

namespace simplecc {
using llvm::Constant;
using llvm::ConstantInt;
using llvm::Value;

/// This class converts values of different types (mostly constants)
/// to LLVM counterparts.
class LLVMValueMap : public LLVMTypeMap {
  llvm::Module &TheModule;

public:
  LLVMValueMap(llvm::Module &M, LLVMContext &Context)
      : LLVMTypeMap(Context), TheModule(M) {}

  /// Convert an int value.
  Constant *getInt(int N) const;

  /// Convert a char value.
  Constant *getChar(int C) const;

  // Convert a constant expression.
  Constant *getConstantFromExpr(ExprAST *E) const;

  Constant *getGlobalInitializer(VarDecl *VD);
};
} // namespace simplecc

#endif // SIMPLECC_LLVM_LLVMVALUEMAP_H

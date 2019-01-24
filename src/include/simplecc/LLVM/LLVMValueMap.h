#ifndef SIMPLECC_LLVM_LLVMVALUEMAP_H
#define SIMPLECC_LLVM_LLVMVALUEMAP_H
#include "simplecc/LLVM/LLVMTypeMap.h"
#include <llvm/IR/Constants.h>

namespace simplecc {
using llvm::Constant;
using llvm::ConstantInt;
using llvm::Value;

/// @brief LLVMValueMap converts values of different types (mostly constants)
/// to their LLVM counterparts.
class LLVMValueMap : public LLVMTypeMap {
  llvm::Module &TheModule;

public:
  LLVMValueMap(llvm::Module &M, LLVMContext &Context)
      : LLVMTypeMap(Context), TheModule(M) {}

  /// @brief Return a Value for an int.
  Constant *getInt(int N) const;

  /// @brief Return a Value for a char.
  Constant *getChar(int C) const;

  /// @brief Return a Value for a constant expression.
  /// Require \param E must be either a NumExpr or CharExpr.
  Constant *getConstantFromExpr(ExprAST *E) const;

  /// @brief Return a Value for global initializer of a variable.
  Constant *getGlobalInitializer(VarDecl *VD);
};

} // namespace simplecc
#endif // SIMPLECC_LLVM_LLVMVALUEMAP_H

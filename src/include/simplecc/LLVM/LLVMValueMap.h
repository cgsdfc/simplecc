#ifndef SIMPLECOMPILER_LLVMVALUEMAP_H
#define SIMPLECOMPILER_LLVMVALUEMAP_H
#include "LLVMTypeMap.h"
#include <llvm/IR/Constants.h>

namespace simplecc {
using llvm::ConstantInt;
using llvm::Constant;
using llvm::Value;

/// This class converts values of different types (mostly constants)
/// to LLVM counterparts.
class LLVMValueMap : public LLVMTypeMap {
  llvm::Module &TheModule;

public:
  LLVMValueMap(llvm::Module &M, LLVMContext &Context)
      : LLVMTypeMap(Context), TheModule(M) {}

  /// Convert a Constant value.
  Value *getConstant(const ConstType &C) const;

  /// Convert a Num node to int value.
  Constant *getInt(int N) const ;

  /// Convert a Char node to char value.
  Constant *getChar(char C) const;

  // Convert a constant expression.
  Constant *getConstantFromExpr(Expr *E) const;

  /// Convert a bool value.
  Constant *getBool(bool B) const;

  Constant *getGlobalInitializer(VarDecl *VD);
};
}

#endif //SIMPLECOMPILER_LLVMVALUEMAP_H

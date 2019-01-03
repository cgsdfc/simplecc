#ifndef SIMPLECC_IR_IRBUILDER_H
#define SIMPLECC_IR_IRBUILDER_H
#include "simplecc/IR/Instructions.h"
#include "simplecc/IR/Constant.h"
#include "simplecc/IR/GlobalVariable.h"
#include "BasicBlock.h"

namespace simplecc {
class IRBuilder {
  template<typename InstTy>
  InstTy *Insert(InstTy *I) const;

public:
  ReturnInst *CreateRetVoid() {
    return Insert(ReturnInst::Create());
  }
  ReturnInst *CreateRet(Value *V) {
    return Insert(ReturnInst::Create(V));
  }

  BranchInst *CreateCondBr(Value *Cond, BasicBlock *True, BasicBlock *False) {
    return Insert(BranchInst::Create(True, False, Cond));
  }
  BranchInst *CreateBr(BasicBlock *Dest) {
    return Insert(BranchInst::Create(Dest));
  }

  CallInst *CreateCall(Value *Callee, const std::vector<Value *> &Args) {
    return CallInst::Create(Callee, Args);
  }

  AllocaInst *CreateAlloca(unsigned NumAlloc) {
    return AllocaInst::Create(NumAlloc);
  }
  LoadInst *CreateLoad(Value *Ptr) {
    return LoadInst::Create(Ptr);
  }
  StoreInst *CreateStore(Value *Val, Value *Ptr) {
    return StoreInst::Create(Ptr, Val);
  }
  GetElementPtrInst *CreateGEP(Value *Ptr, Value *Idx) {
    return GetElementPtrInst::Create(Ptr, Idx);
  }

#define HANDLE_BINARY_OPERATOR(Class, Opcode, Name) \
BinaryOperator *Create##Opcode(Value *LHS, Value *RHS) {       \
  return BinaryOperator::Create##Opcode(LHS, RHS);                                    \
  }
#include "simplecc/IR/Instruction.def"

#define HANDLE_ICMP_PREDICATE(Class, Opcode, Name)                             \
   ICmpInst *CreateICmp##Opcode(Value *LHS, Value *RHS) {   \
    return ICmpInst::Create##Opcode(LHS, RHS);                                \
  }
#include "simplecc/IR/Instruction.def"

  BinaryOperator *CreateNeg(Value *S) {
    return BinaryOperator::CreateNeg(Context, S);
  }

  PHINode *CreatePHI() {
    Insert(PHINode::Create());
  }

  void ClearInsertPoint() {
    BB = nullptr;
    InsertPt = BasicBlock::iterator();
  }

  BasicBlock *GetInsertBlock() const { return BB; }
  BasicBlock::iterator GetInsertPoint() const { return InsertPt; }
  Function *GetCurrentFunction() const {
    assert(BB && "No BB associated");
    return BB->getParent();
  }

  void SetInsertPoint(BasicBlock *TheBB);
  void SetInsertPoint(Instruction *I);
  void SetInsertPoint(BasicBlock *TheBB, BasicBlock::iterator IP);
  Type *getCurrentFunctionReturnType() const;

  StringLiteral *getStringLiteral(const std::string &Str) {
    return StringLiteral::get(Context, Str);
  }
  ConstantInt *getTrue() {
    return getInt(1);
  }
  ConstantInt *getFalse() {
    return getInt(0);
  }
  ConstantInt *getInt(int C) {
    return ConstantInt::get(Context, C);
  }

  Type *getIntType() const {
    return Type::getIntType();
  }
  Type *getVoidType() const {
    return Type::getVoidType();
  }
  Type *getPointerType() const {
    return Type::getPointerType();
  }

  IRBuilder(IRContext &C, BasicBlock *TheBB) : Context(C) {
    SetInsertPoint(TheBB);
  }
  ~IRBuilder() = default;
private:
  IRContext &Context;
  BasicBlock *BB;
  BasicBlock::iterator InsertPt;
};

template<typename InstTy>
InstTy *IRBuilder::Insert(InstTy *I) const {
  if (BB) {
    BB->getInstList().insert(InsertPt, I);
  }
  return I;
}

} // namespace simplecc

#endif // SIMPLECC_IR_IRBUILDER_H
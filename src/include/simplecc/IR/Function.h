#ifndef SIMPLECC_IR_FUNCTION_H
#define SIMPLECC_IR_FUNCTION_H
#include <simplecc/Support/iterator_range.h>
#include "simplecc/IR/Value.h"

namespace simplecc {
class Argument;
class Module;
class BasicBlock;

/// The Function class is merely an ordered list of BasicBlocks.
class Function final : public Value {
  friend class Value;
  Function(Type *ReturnType, unsigned NumArgs, Module *M);
  ~Function();
public:
  using BasicBlockListType = std::vector<BasicBlock *>;
  using iterator = BasicBlockListType::iterator;
  using const_iterator = BasicBlockListType::const_iterator;

  using ArgumentListType = std::vector<Argument *>;
  using arg_iterator = ArgumentListType::iterator;
  using const_arg_iterator = ArgumentListType::const_iterator;

  size_t size() const { return BasicBlocks.size(); }
  bool empty() const { return BasicBlocks.empty(); }
  BasicBlock &front() { return *BasicBlocks.front(); }
  const BasicBlock &front() const { return *BasicBlocks.front(); }
  BasicBlock &back() { return *BasicBlocks.back(); }
  const BasicBlock &back() const { return *BasicBlocks.back(); }

  iterator begin() { return BasicBlocks.begin(); }
  iterator end() { return BasicBlocks.end(); }
  const_iterator begin() const { return BasicBlocks.begin(); }
  const_iterator end() const { return BasicBlocks.end(); }

  arg_iterator arg_begin() { return Arguments.begin(); }
  arg_iterator arg_end() { return Arguments.end(); }
  const_arg_iterator arg_begin() const { return Arguments.begin(); }
  const_arg_iterator arg_end() const { return Arguments.end(); }

  iterator_range<arg_iterator> args() {
    return make_range(arg_begin(), arg_end());
  }
  iterator_range<const_arg_iterator> args() const {
    return make_range(arg_begin(), arg_end());
  }

  size_t arg_size() const { return Arguments.size(); }
  bool arg_empty() const { return Arguments.empty(); }

  const BasicBlock &getEntryBlock() const { return front(); }
  BasicBlock &getEntryBlock() { return front(); }
  BasicBlockListType &getBasicBlockList() { return BasicBlocks; }
  const BasicBlockListType &getBasicBlockList() const { return BasicBlocks; }

  unsigned getInstructionCount() const;
  Type *getReturnType() const { return ReturnType; }

  Function(const Function &) = delete;
  Function &operator=(const Function &) = delete;

  static Function *Create(Type *RetTy, unsigned NumArgs, const std::string &Name, Module &M) {
    return new Function(RetTy, NumArgs, &M);
  }

  Module *getParent() const { return Parent; }
  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == FunctionVal;
  }
  void eraseFromParent();
private:
  Module *Parent;
  Type *ReturnType;
  ArgumentListType Arguments;
  BasicBlockListType BasicBlocks;
};
} // namespace simplecc

#endif
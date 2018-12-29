#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H
#include "simplecc/IR/Value.h"

namespace simplecc {
class Instruction;
class Function;

class BasicBlock final : public Value {
  Function *Parent;
  std::vector<Instruction *> InstList;

public:
  BasicBlock(const BasicBlock &) = delete;
  BasicBlock &operator=(const BasicBlock &) = delete;

  Function *getParent() const { return Parent; }

  /// Iterator interface to InstList.
  using InstListType = decltype(InstList);
  using iterator = InstListType::iterator;
  using const_iterator = InstListType::const_iterator;
  using reverse_iterator = InstListType::reverse_iterator;
  using const_reverse_iterator = InstListType::const_reverse_iterator;

  iterator begin() { return InstList.begin(); }
  iterator end() { return InstList.end(); }

  const_iterator begin() const { return InstList.begin(); }
  const_iterator end() const { return InstList.end(); }

  reverse_iterator rbegin() { return InstList.rbegin(); };
  reverse_iterator rend() { return InstList.rend(); }

  const_reverse_iterator rbegin() const { return InstList.rbegin(); }
  const_reverse_iterator rend() const { return InstList.rend(); }

  bool empty() const { return InstList.empty(); }
  size_t size() const { return InstList.size(); }

  Instruction &front() { return *InstList.front(); };
  const Instruction &front() const { return *InstList.front(); }

  Instruction &back() { return *InstList.back(); }
  const Instruction &back() const { return *InstList.back(); }

  InstListType &getInstList() { return InstList; }
  const InstListType &getInstList() const { return InstList; }

  Instruction *getTerminator();
  const Instruction *getTerminator() const {
    return const_cast<BasicBlock *>(this)->getTerminator();
  }

  static bool InstanceCheck(const Value *V) {
    return V->getValueID() == Value::BasicBlockVal;
  }

  void removeInst(const Instruction *I);

};
} // namespace simplecc

#endif

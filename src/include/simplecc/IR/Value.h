#ifndef SIMPLECC_IR_VALUE_H
#define SIMPLECC_IR_VALUE_H
#include "simplecc/IR/Use.h"
#include "simplecc/IR/Type.h"

#include <string>
#include <vector>
#include <simplecc/Support/iterator_range.h>

namespace simplecc {
/// This class represents a value in the SSA form.
/// It maintains a list of Use, which holds all the users of this value.
class Value {
  friend class Use;
public:
  using UseListType = std::vector<Use>;
  using use_iterator = UseListType::iterator;
  using const_use_iterator = UseListType::const_iterator;

  use_iterator use_begin() { return UseList.begin(); }
  use_iterator use_end() { return UseList.end(); }
  const_use_iterator use_begin() const { return UseList.begin(); }
  const_use_iterator use_end() const { return UseList.end(); }

  bool use_empty() const { return UseList.empty(); }
  size_t use_size() const { return UseList.size(); }

  iterator_range<use_iterator> uses() {
    return make_range(use_begin(), use_end());
  }
  iterator_range<const_use_iterator> uses() const {
    return make_range(use_begin(), use_end());
  }

  /// Subclasses kind of Value.
  enum ValueKind : unsigned {
    ConstantVal,
    FunctionVal,
    BasicBlockVal,
    ArgumentVal,
    InstructionVal,
  };

  Value(const Value &) = delete;
  Value &operator=(const Value &) = delete;

  unsigned getValueID() const { return Kind; }
  Type *getType() const { return Ty; }

  void deleteValue();
protected:
  Value(Type *Ty, unsigned K) : Kind(K), Ty(Ty) {}
  /// Add a User identified by U.
  void addUse(const Use &U);
  /// Remove a User identified by U.
  void removeUse(const Use &U);
  ~Value();

private:
  const unsigned Kind;
  Type *Ty;
  std::vector<Use> UseList;
};

struct ValueDeleter {
  void operator()(Value *Val) {
    Val->deleteValue();
  }
};

}

#endif //SIMPLECC_IR_VALUE_H

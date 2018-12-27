#ifndef SIMPLECC_OPTIMIZE_VALUE_H
#define SIMPLECC_OPTIMIZE_VALUE_H
#include "simplecc/Optimize/Use.h"
#include "simplecc/Optimize/Type.h"

#include <string>
#include <vector>

namespace simplecc {
/// This class represents a value in the SSA form.
class Value {
  unsigned Kind;
  Type *Ty;
  std::string Name;
  std::vector<Use> UseList;

public:
  /// Iterator interface to the UseList.
  using use_iterator = decltype(UseList)::iterator;
  using const_use_iterator = decltype(UseList)::const_iterator;

  use_iterator use_begin() { return UseList.begin(); }
  use_iterator use_end() { return UseList.end(); }

  const_use_iterator use_begin() const { return UseList.begin(); }
  const_use_iterator use_end() const { return UseList.end(); }

  bool use_empty() const { return UseList.empty(); }
  unsigned use_size() const { return UseList.size(); }

  /// Subclasses kind of Value.
  enum ValueKind : unsigned {
    ConstantVal,
    FunctionVal,
    BasicBlockVal,
    ArgumentVal,
    InstructionVal,
  };

  unsigned getValueID() const { return Kind; }

  void setName(std::string N) { Name = std::move(N); }
  std::string getName() const { return Name; }

  Type *getType() const { return Ty; }

protected:
  Value(Type *Ty, unsigned K) : Kind(K), Ty(Ty) {}
};

}

#endif //SIMPLECC_OPTIMIZE_VALUE_H

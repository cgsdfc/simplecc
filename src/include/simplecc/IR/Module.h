#ifndef SIMPLECC_IR_MODULE_H
#define SIMPLECC_IR_MODULE_H
#include <simplecc/Support/iterator_range.h>
#include <string>
#include <vector>

namespace simplecc {
class Function;
class GlobalVariable;

/// The Module class is a containers of a list of GlobalValues and a list of
/// Functions.
class Module final {
public:
  using FunctionListType = std::vector<Function *>;
  using iterator = FunctionListType::iterator;
  using const_iterator = FunctionListType::const_iterator;
  using reverse_iterator = FunctionListType::reverse_iterator;
  using const_reverse_iterator = FunctionListType::const_reverse_iterator;

  using GlobalListType = std::vector<GlobalVariable *>;
  using global_iterator = GlobalListType::iterator;
  using const_global_iterator = GlobalListType::const_iterator;

  iterator begin() { return FunctionList.begin(); }
  const_iterator begin() const { return FunctionList.begin(); }
  iterator end() { return FunctionList.end(); }
  const_iterator end() const { return FunctionList.end(); }
  reverse_iterator rbegin() { return FunctionList.rbegin(); }
  reverse_iterator rend() { return FunctionList.rend(); }
  const_reverse_iterator rbegin() const { return FunctionList.rbegin(); }
  const_reverse_iterator rend() const { return FunctionList.rend(); }

  iterator_range<iterator> functions() { return make_range(begin(), end()); }
  iterator_range<const_iterator> functions() const {
    return make_range(begin(), end());
  }

  bool empty() const { return FunctionList.empty(); }
  size_t size() const { return FunctionList.size(); }

  global_iterator global_begin() { return GlobalList.begin(); }
  global_iterator global_end() { return GlobalList.end(); }
  const_global_iterator global_begin() const { return GlobalList.begin(); }
  const_global_iterator global_end() const { return GlobalList.end(); }
  bool global_empty() const { return GlobalList.empty(); }
  size_t global_size() const { return GlobalList.size(); }

  iterator_range<global_iterator> globals() {
    return make_range(global_begin(), global_end());
  }
  iterator_range<const_global_iterator> globals() const {
    return make_range(global_begin(), global_end());
  }

  FunctionListType &getFunctionList() { return FunctionList; }
  const FunctionListType &getFunctionList() const { return FunctionList; }

  GlobalListType &getGlobalList() { return GlobalList; }
  const GlobalListType &getGlobalList() const { return GlobalList; }

  const std::string &getModuleIdentifier() const { return ModuleID; }
  unsigned getInstructionCount();

  explicit Module(std::string ModuleID);
  ~Module();

private:
  std::string ModuleID;
  GlobalListType GlobalList;
  FunctionListType FunctionList;
};

} // namespace simplecc
#endif

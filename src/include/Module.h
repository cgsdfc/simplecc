#ifndef MODULE_H
#define MODULE_H
#include <string>
#include <vector>

namespace simplecompiler {
class Function;

class Module {
  std::string Name;
  std::vector<Function *> Functions;
  std::vector<std::string> Strings;

public:
  explicit Module(std::string Name);

  std::vector<std::string> &getStrings() { return Strings; }
  const std::vector<std::string> &getStrings() const { return Strings; }

  std::vector<Function *> &getFunctions() { return Functions; }
  const std::vector<Function *> &getFunctions() const { return Functions; }

  /// Function iteration.
  using iterator = decltype(Functions)::iterator;
  using const_iterator = decltype(Functions)::const_iterator;
  iterator begin() { return Functions.begin(); }
  iterator end() { return Functions.end(); }
  const_iterator begin() const { return Functions.begin(); }
  const_iterator end() const { return Functions.end(); }

  bool empty() const { return Functions.empty(); }
  size_t size() const { return Functions.size(); }
};

} // namespace simplecompiler
#endif

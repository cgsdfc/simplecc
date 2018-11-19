#include "tokenize.h"

template <typename... Args>
void Print(std::ostream &os, Args&&... args);

template <>
inline void Print(std::ostream &os) {
  os << "\n";
}

template <typename Last>
inline void Print(std::ostream &os, Last&& last) {
  os << last << "\n";
}

template <typename First, typename... Rest>
inline void Print(std::ostream &os, First&& first, Rest&&... rest) {
  os << first << " ";
  Print(os, rest...);
}

template <typename... Args>
inline void Error(const Location &loc, Args&&... args) {
  Print(std::cerr,
      "Error in line", loc.lineno, "column", loc.col_offset, ":", args...);
}


class ErrorManager {
public:
  int error_count;
  ErrorManager(): error_count(0) {}

  template <typename... Args>
    void Error(const Location &loc, Args&&... args) {
      ::Error(loc, args...);
      ++error_count;
    }

  int GetErrorCount() const { return error_count; }

  bool IsOk() const { return 0 == GetErrorCount(); }
};

inline String Quote(const String &string) {
  return '\'' + string + '\'';
}

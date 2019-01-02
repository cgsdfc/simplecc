#ifndef SIMPLECC_LEX_LOCATION_H
#define SIMPLECC_LEX_LOCATION_H
#include <iostream>

namespace simplecc {
class Location {
  unsigned Line;
  unsigned Column;

public:
  Location() : Line(0), Column(0) {}
  Location(unsigned lineno, unsigned col_offset)
      : Line(lineno), Column(col_offset) {}

  unsigned getLineNo() const { return Line; }

  unsigned getColOffset() const { return Column; }

  void Format(std::ostream &O) const;

  void FormatCompact(std::ostream &O) const;
};

inline std::ostream &operator<<(std::ostream &os, const Location &loc) {
  loc.Format(os);
  return os;
}
} // namespace simplecc

#endif // SIMPLECC_LEX_LOCATION_H
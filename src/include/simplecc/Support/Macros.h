#ifndef SIMPLECC_SUPPORT_MACROS_H
#define SIMPLECC_SUPPORT_MACROS_H
#include <iostream>

/// Define an inline output operator for a class that implements a Format()
/// method.
/// We usually need an output operator for an object to be able to write to stream
/// and we put logic in the Format() method.
#define DEFINE_INLINE_OUTPUT_OPERATOR(Class) \
inline std::ostream &operator<<(std::ostream &O, const Class &X) { \
  X.Format(O); \
  return O; \
}

#endif //SIMPLECC_SUPPORT_MACROS_H
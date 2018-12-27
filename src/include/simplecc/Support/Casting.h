#ifndef SIMPLECC_SUPPORT_CASTING_H
#define SIMPLECC_SUPPORT_CASTING_H

namespace simplecc {
template<typename T, typename U> bool IsInstance(U *x) {
  return T::InstanceCheck(x);
}

template<typename T, typename U> T *subclass_cast(U *x) {
  if (IsInstance<T>(x))
    return static_cast<T *>(x);
  return nullptr;
}
}

#endif //SIMPLECC_SUPPORT_CASTING_H

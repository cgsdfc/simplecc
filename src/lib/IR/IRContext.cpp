#include <simplecc/IR/IRContext.h>

using namespace simplecc;

IRContext::IRContext() : VoidTy(Type::VoidTypeKind),
                         LabelTy(Type::LabelTypeKind),
                         IntTy(Type::IntTypeKind),
                         PointerType(Type::PointerTypeKind) {

}

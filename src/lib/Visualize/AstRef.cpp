#include "simplecc/Visualize/AstRef.h"
#include "simplecc/Parse/AST.h"

#include <cassert>
#include <llvm/Support/ErrorHandling.h>

using namespace simplecc;

/// Wrap AST::GetClassName().
const char *AstRef::getClassName() const {
  assert(Ref);
  return Ref->GetClassName();
}

/// Return the Location of the wrapped AST node.
const Location &AstRef::getLocation() const {
  switch (Kind) {
  default:
    llvm_unreachable("Unhandled AST subclass!");
#define HANDLE_AST_TYPE(NAME)                                                  \
  case AstKind::NAME:                                                          \
    return static_cast<NAME *>(Ref)->getLoc();
    HANDLE_AST_TYPE(Decl)
    HANDLE_AST_TYPE(Stmt)
    HANDLE_AST_TYPE(Expr)
#undef HANDLE_AST_TYPE
  }
}
#ifndef AST_VERIFIER_H
#define AST_VERIFIER_H

namespace simplecc {
class Program;
/// This ensure that the AST is well-formed.
/// In terms of well-formness:
///
bool VerifyAST(Program *P);
} // namespace simplecc
#endif

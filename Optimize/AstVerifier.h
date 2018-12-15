#ifndef AST_VERIFIER_H
#define AST_VERIFIER_H

namespace simplecompiler {
class Program;
/// This ensure that the AST is well-formed.
/// In terms of well-formness:
///
bool VerifyAST(Program *P);
} // namespace simplecompiler
#endif

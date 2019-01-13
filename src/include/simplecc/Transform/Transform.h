/// @file External interface of Transform module.
// This is the interface to the Transform module, which performs various cleanup
/// operations on the AST. Since it makes changes to the AST, it is called Transform
/// rather than Analysis.
#ifndef SIMPLECC_TRANSFORM_TRANSFORM_H
#define SIMPLECC_TRANSFORM_TRANSFORM_H

namespace simplecc {
class ProgramAST;
class SymbolTable;

/// This function performs all the transformations on the AST.
void TransformProgram(ProgramAST *P, SymbolTable &S);
} // namespace simplecc
#endif // SIMPLECC_TRANSFORM_TRANSFORM_H
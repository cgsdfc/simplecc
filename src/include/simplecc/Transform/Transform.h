#ifndef SIMPLECC_TRANSFORM_TRANSFORM_H
#define SIMPLECC_TRANSFORM_TRANSFORM_H
/// This is the interface to the Transform module, which performs various cleanup
/// operations on the AST. Since it makes changes to the AST, it is called Transform
/// rather than Analysis.
namespace simplecc {
class ProgramAST;
class SymbolTable;
void TransformProgram(ProgramAST *P, SymbolTable &S);
} // namespace simplecc
#endif // SIMPLECC_TRANSFORM_TRANSFORM_H
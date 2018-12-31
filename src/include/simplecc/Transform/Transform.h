#ifndef SIMPLECC_TRANSFORM_TRANSFORM_H
#define SIMPLECC_TRANSFORM_TRANSFORM_H
namespace simplecc {
class Program;
class SymbolTable;
void TransformProgram(Program *P, SymbolTable &S);
} // namespace simplecc
#endif // SIMPLECC_TRANSFORM_TRANSFORM_H
#ifndef IMPLICIT_CALL_TRANSFORMER_H
#define IMPLICIT_CALL_TRANSFORMER_H

namespace simplecompiler {
class Program;
class SymbolTable;

void TransformImplicitCall(Program *P, const SymbolTable &S);
} // namespace simplecompiler
#endif

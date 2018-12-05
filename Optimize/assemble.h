#ifndef ASSEMBLE_H
#define ASSEMBLE_H
#include <iostream>

namespace simplecompiler {
class CompiledModule;
void AssembleMips(const CompiledModule &module, std::ostream &os);
}
#endif

#ifndef ASSEMBLE_H
#define ASSEMBLE_H
#include "compile.h"
#include <iostream>

void AssembleMips(const CompiledModule &module, std::ostream &os);
#endif
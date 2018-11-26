#ifndef MIPS_H
#define MIPS_H
#include "compile.h"
#include <iostream>

void AssembleMips(const CompiledModule &module, std::ostream &os);
#endif

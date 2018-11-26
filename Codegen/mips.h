#ifndef MIPS_H
#define MIPS_H
#include <iostream>
#include "compile.h"

void AssembleMips(const CompiledModule &module, std::ostream &os);
#endif

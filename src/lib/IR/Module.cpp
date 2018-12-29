#include "simplecc/IR/Module.h"
#include <utility>

using namespace simplecc;

Module::Module(std::string Name) : Name(std::move(Name)), Functions() {}

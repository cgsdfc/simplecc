#include "simplecc/Module.h"
#include <string>
#include <utility>

using namespace simplecc;

Module::Module(std::string Name) : Name(std::move(Name)), Functions() {}

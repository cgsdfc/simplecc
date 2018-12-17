#include "simplecompiler/Module.h"
#include <string>
#include <utility>

using namespace simplecompiler;

Module::Module(std::string Name) : Name(std::move(Name)), Functions() {}

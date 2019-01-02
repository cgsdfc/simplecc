#include <numeric>
#include <simplecc/IR/Function.h>
#include <simplecc/IR/Module.h>

using namespace simplecc;

Module::Module(std::string ModuleID) : ModuleID(ModuleID) {}

/// Count all the Instructions within this Module.
unsigned Module::getInstructionCount() {
  return std::accumulate(begin(), end(), 0U,
                         [](unsigned Count, const Function *F) {
                           return Count + F->getInstructionCount();
                         });
}

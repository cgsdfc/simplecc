#include <simplecc/IR/Module.h>
#include <simplecc/IR/Function.h>
#include <numeric>

using namespace simplecc;

Module::Module(std::string ModuleID) : ModuleID(ModuleID) {}

/// Count all the Instructions within this Module.
unsigned Module::getInstructionCount() {
  return std::accumulate(begin(), end(), static_cast<unsigned >(0), [](unsigned Count, const Function *F) {
    return Count + F->getInstructionCount();
  });
}

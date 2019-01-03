#include "simplecc/IR/Module.h"
#include "simplecc/IR/Function.h"
#include "simplecc/IR/GlobalVariable.h"
#include <numeric>
#include <utility>
#include <algorithm>

using namespace simplecc;

Module::Module(std::string ModuleID) : ModuleID(std::move(ModuleID)) {}

/// Count all the Instructions within this Module.
unsigned Module::getInstructionCount() {
  return std::accumulate(begin(), end(), 0U,
                         [](unsigned Count, const Function *F) {
                           return Count + F->getInstructionCount();
                         });
}

Module::~Module() {
  std::for_each(global_begin(), global_end(), ValueDeleter());
  std::for_each(begin(), end(), ValueDeleter());
}

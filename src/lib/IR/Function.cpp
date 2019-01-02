#include <algorithm>
#include <numeric>
#include <simplecc/IR/Argument.h>
#include <simplecc/IR/BasicBlock.h>
#include <simplecc/IR/Function.h>
#include <simplecc/IR/Module.h>

using namespace simplecc;

unsigned Function::getInstructionCount() const {
  return std::accumulate(
      begin(), end(), 0U,
      [](unsigned Count, const BasicBlock *BB) { return Count + BB->size(); });
}

Function::Function(Type *ReturnType, unsigned NumArgs, Module *M)
    : Value(Type::getFunctionType(), FunctionVal), ReturnType(ReturnType) {
  // Construct the Arguments.
  for (unsigned I = 0; I < NumArgs; I++) {
    Arguments.push_back(Argument::Create(*this, I));
  }
  if (M) {
    M->getFunctionList().push_back(this);
  }
}

Function::~Function() {
  std::for_each(arg_begin(), arg_end(), ValueDeleter());
  std::for_each(begin(), end(), ValueDeleter());
}

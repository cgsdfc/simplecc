
#include <simplecc/IR/IRPrinter.h>

#include "simplecc/IR/IRPrinter.h"
#include "simplecc/IR/Module.h"
#include "simplecc/IR/Function.h"

using namespace simplecc;

void IRPrinter::printModule(const Module &M) {
  OS << "; ModuleID = '" << M.getModuleIdentifier() << "'\n";
  OS << "\n";

  printValueList(M.global_begin(), M.global_end(), [this](const GlobalVariable *V){
    printGlobalVariable(*V);
  });
  OS << "\n";

  printValueList(M.begin(), M.end(), [this](const Function *F) {
    printFunction(*F);
  });
  OS << "\n";
}

void IRPrinter::printFunction(const Function &F) {
  OS << "define " << F.getReturnType()->getName() << " @" << F.getName();

  printValueList(F.begin(), F.end(), [this](const BasicBlock *BB) {
    printBasicBlock(*BB);
  });
  OS << "}";
}

void IRPrinter::printGlobalVariable(const GlobalVariable &Var) {
  OS << "@" << Var.getName() << " = " << "global int size " << Var.getSize();
}

void IRPrinter::printBasicBlock(const BasicBlock &BB) {
  increaseIndentLevel();

  decreaseIndentLevel();
}


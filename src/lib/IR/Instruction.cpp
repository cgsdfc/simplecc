#include "simplecc/IR/BasicBlock.h"
#include "simplecc/IR/Function.h"
#include <simplecc/IR/Instruction.h>

using namespace simplecc;

Instruction::Instruction(Type *Ty, unsigned Opcode, unsigned NumOps,
                         BasicBlock *InsertAtEnd)
    : User(Ty, Opcode + InstructionVal, NumOps), Parent(InsertAtEnd) {
  if (InsertAtEnd) {
    InsertAtEnd->getInstList().push_back(this);
  }
}

const char *Instruction::getOpcodeName(unsigned Opcode) {
  switch (Opcode) {
#define HANDLE_INSTRUCTION(Class, Opcode, Name)                                \
  case Opcode:                                                                 \
    return Name;
#include "simplecc/IR/Instruction.def"
  default:
    assert(false && "Invalid Opcode");
  }
}

Instruction *Instruction::cloneImpl() const {
  assert(false && "subclass should implement cloneImpl()");
  return nullptr;
}

bool Instruction::isTerminator(unsigned Opcode) {
  switch (Opcode) {
#define HANDLE_TERMINATOR(Class, Opcode, Name)                                 \
  case Opcode:                                                                 \
    return true;
#include "simplecc/IR/Instruction.def"
  default:
    return false;
  }
}

bool Instruction::isBinaryOp(unsigned Opcode) {
  switch (Opcode) {
#define HANDLE_BINARY_OPERATOR(Class, Opcode, Name)                            \
  case Opcode:                                                                 \
    return true;
#include "simplecc/IR/Instruction.def"
  default:
    return false;
  }
}

const Function *Instruction::getFunction() const {
  assert(Parent && "Parent not set!");
  return Parent->getParent();
}

const Module *Instruction::getModule() const {
  return getFunction()->getParent();
}

Instruction::~Instruction() {}

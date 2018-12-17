#ifndef INSTRUCTION_H
#define INSTRUCTION_H

namespace simplecc {
class Instruction {
  unsigned Opcode;

protected:
  Instruction(unsigned Opcode) : Opcode(Opcode) {}

public:
  unsigned getOpcode() const { return Opcode; }
};

} // namespace simplecc

#endif

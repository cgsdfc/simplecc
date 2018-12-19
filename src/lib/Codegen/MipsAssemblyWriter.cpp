#include "simplecc/Codegen/MipsAssemblyWriter.h"
#include "simplecc/Codegen/ByteCodeFunction.h"
#include "simplecc/Codegen/ByteCodeModule.h"
#include "simplecc/Codegen/ByteCodeToMipsTranslator.h"
#include "simplecc/Codegen/MipsSupport.h"
#include "simplecc/Analysis/Types.h" // SymbolEntry

#include <iostream>
#include <numeric> // accumulate()

using namespace simplecc;

void MipsAssemblyWriter::WriteData(Printer &W, const ByteCodeModule &Module) {
  W.WriteLine(".data");
  W.WriteLine("# Global objects");

  for (const SymbolEntry &E : Module.GetGlobalVariables()) {
    GlobalLabel GL(E.getName(), /* NeedColon */ true);
    if (!E.IsArray()) {
      W.WriteLine(GL, ".word", 0);
      continue;
    }
    W.WriteLine(GL, ".space", BytesFromEntries(E.AsArray().getSize()));
  }

  W.WriteLine();
  W.WriteLine("# String literals");

  for (const std::pair<String, unsigned> &Item :
       Module.GetStringLiteralTable()) {
    W.WriteLine(AsciizLabel(Item.second, /* NeedColon */ true), ".asciiz",
                EscapedString(Item.first));
  }

  W.WriteLine("# End of data segment");
}

// Return the total bytes consumed by local objects, including
// variables, arrays and formal arguments.
int MipsAssemblyWriter::getLocalObjectsInBytes(
    const ByteCodeFunction &TheFunction) const {
  unsigned Entries = std::accumulate(
      TheFunction.local_begin(), TheFunction.local_end(),
      TheFunction.GetFormalArgumentCount(),
      [](unsigned Entries, const SymbolEntry &E) {
        return Entries + (E.IsArray() ? E.AsArray().getSize() : 1);
      });
  return BytesFromEntries(Entries);
}

void MipsAssemblyWriter::WriteFunction(Printer &W,
                                       const ByteCodeFunction &TheFunction) {
  TheContext.Initialize(TheFunction);
  ByteCodeToMipsTranslator TheTranslator(W.getOuts(), TheContext);

  WritePrologue(W, TheFunction);
  for (const ByteCode &C : TheFunction) {
    TheTranslator.Write(C);
  }
  WriteEpilogue(W, TheFunction);
}

void MipsAssemblyWriter::WriteText(Printer &W, const ByteCodeModule &Module) {
  W.WriteLine(".text");
  W.WriteLine(".globl main");
  W.WriteLine("jal main");
  W.WriteLine("li $v0,", MipsSyscallCode::EXIT_PROGRAM);
  W.WriteLine("syscall");
  W.WriteLine();
  W.WriteLine("# User defined functions");

  for (const ByteCodeFunction *Fn : Module) {
    WriteFunction(W, *Fn);
    W.WriteLine();
  }
  W.WriteLine("# End of text segment");
}

void MipsAssemblyWriter::WritePrologue(Printer &W,
                                       const ByteCodeFunction &TheFunction) {
  W.WriteLine(GlobalLabel(TheFunction.getName(), /* NeedColon */ true));
  W.WriteLine("# Prologue");
  W.WriteLine("sw $ra, 0($sp)");
  W.WriteLine("sw $fp, -4($sp)");
  W.WriteLine("move $fp, $sp");
  W.WriteLine("addi $sp, $sp,", -BytesFromEntries(2));
  W.WriteLine();

  auto NumArgs = TheFunction.GetFormalArgumentCount();
  if (NumArgs) {
    // copy arguments here
    W.WriteLine("# Passing Arguments");
    for (int I = 0; I < NumArgs; I++) {
      // actual is above $fp, with $fp + 4 pointing to the last arg.
      signed int Actual = BytesFromEntries(NumArgs - I);
      // formal is under $sp, with $sp pointing to the first arg.
      signed int Formal = BytesFromEntries(-I);
      W.WriteLine("lw $t0,", Actual, "($fp)");
      W.WriteLine("sw $t0,", Formal, "($sp)");
    }
    W.WriteLine();
  }

  signed Off = getLocalObjectsInBytes(TheFunction);
  if (Off != 0) {
    W.WriteLine("# Make room for local objects");
    W.WriteLine("addi $sp, $sp,", -Off);
    W.WriteLine();
  }
  // now $fp points to the bottom of stack,
  // $sp points to the next TOS of the stack.
  // $ra stored at 0($fp)
  // $fp stored at -4($fp)
}

void MipsAssemblyWriter::WriteEpilogue(Printer &W,
                                       const ByteCodeFunction &TheFunction) {
  W.WriteLine("# Epilogue");
  W.WriteLine(ReturnLabel(TheFunction.getName(), /* NeedColon */ true));
  W.WriteLine("lw $ra, 0($fp)");
  W.WriteLine("move $sp, $fp");
  W.WriteLine("lw $fp, -4($fp)");
  W.WriteLine("jr $ra");
}

void MipsAssemblyWriter::Write(const ByteCodeModule &M, std::ostream &O) {
  Printer ThePrinter(O);
  WriteData(ThePrinter, M);
  ThePrinter.WriteLine();
  WriteText(ThePrinter, M);
}

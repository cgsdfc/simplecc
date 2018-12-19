#include "simplecc/Target/LocalContext.h"
#include "simplecc/Codegen/ByteCode.h"
#include "simplecc/Codegen/ByteCodeFunction.h"
#include "simplecc/Target/MipsSupport.h" // BytesFromEntries()

using namespace simplecc;

/// Initialize the **local offset** dictionary for a function.
/// The LocalOffsets is where local variables live on the stack.
void LocalContext::InitializeLocalOffsets() {
  // offset points to the first vacant byte after storing
  // $ra and $fp. $ra is at 0($fp), $fp is at -4($fp)
  LocalOffsets.clear();
  signed Off = -BytesFromEntries(2);

  /// Allocate space for formal arguments.
  for (const SymbolEntry &Arg : TheFunction->GetFormalArguments()) {
    LocalOffsets.emplace(Arg.getName(), Off);
    Off -= BytesFromEntries(1);
  }

  /// Allocate space for non-arg local variables.
  for (const SymbolEntry &Var : TheFunction->GetLocalVariables()) {
    if (Var.IsArray()) {
      Off -= BytesFromEntries(Var.AsArray().getSize());
      LocalOffsets.emplace(Var.getName(), Off + BytesFromEntries(1));
      continue;
    }
    /// Variable:
    assert(Var.IsVariable() && "Local objects must be Variable or Array");
    LocalOffsets.emplace(Var.getName(), Off);
    Off -= BytesFromEntries(1);
  }
}

/// Initialize the **jump targets** set, which tells us whether an offset
/// in the ByteCode stream is a target of some jump command.
void LocalContext::InitializeJumpTargets() {
  JumpTargets.clear();
  for (const ByteCode &C : *TheFunction) {
    if (C.IsJumpXXX()) {
      JumpTargets.insert(C.getIntOperand());
    }
  }
}

// Return the offset of local name relatited to frame pointer
signed int LocalContext::getLocalOffset(const char *Name) const {
  assert(LocalOffsets.count(Name) && "Undefined Name");
  return LocalOffsets.find(Name)->second;
}

// Return whether a name is a variable
bool LocalContext::IsVariable(const char *Name) const {
  return TheFunction->GetLocal()[Name].IsVariable();
}

// Return whether a name is an array
bool LocalContext::IsArray(const char *Name) const {
  return TheFunction->GetLocal()[Name].IsArray();
}

const String &LocalContext::getName() const { return TheFunction->getName(); }

/// Initialize both LocalOffsets and JumpTargets.
void LocalContext::Initialize(const ByteCodeFunction &F) {
  TheFunction = &F;
  InitializeLocalOffsets();
  InitializeJumpTargets();
}

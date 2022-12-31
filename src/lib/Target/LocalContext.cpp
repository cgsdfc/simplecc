// MIT License
// 
// Copyright (c) 2018 Cong Feng.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simplecc/Target/LocalContext.h"
#include "simplecc/CodeGen/ByteCode.h"
#include "simplecc/CodeGen/ByteCodeFunction.h"
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
  for (const SymbolEntry &Arg : TheFunction->getFormalArguments()) {
    LocalOffsets.emplace(Arg.getName(), Off);
    Off -= BytesFromEntries(1);
  }

  /// Allocate space for non-arg local variables.
  for (const SymbolEntry &Var : TheFunction->getLocalVariables()) {
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
    if (C.IsJump()) {
      JumpTargets.insert(C.getJumpTarget());
    }
  }
}

// Return the offset of local name related to frame pointer
signed int LocalContext::getLocalOffset(const char *Name) const {
  assert(LocalOffsets.count(Name) && "Undefined Name");
  return LocalOffsets.find(Name)->second;
}

// Return whether a name is a variable
bool LocalContext::IsVariable(const char *Name) const {
  return TheFunction->getLocalTable()[Name].IsVariable();
}

// Return whether a name is an array
bool LocalContext::IsArray(const char *Name) const {
  return TheFunction->getLocalTable()[Name].IsArray();
}

const std::string &LocalContext::getFuncName() const {
  return TheFunction->getName();
}

/// Initialize both LocalOffsets and JumpTargets.
void LocalContext::Initialize(const ByteCodeFunction &F) {
  TheFunction = &F;
  InitializeLocalOffsets();
  InitializeJumpTargets();
}
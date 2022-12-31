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

#ifndef SIMPLECC_TARGET_LOCALCONTEXT_H
#define SIMPLECC_TARGET_LOCALCONTEXT_H
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace simplecc {
class ByteCodeFunction;

/// This class provides local information for ByteCodeToMipsTranslator
class LocalContext {

  /// Initialize the **local offset** dictionary for a function.
  /// The LocalOffsets is where local variables live on the stack.
  void InitializeLocalOffsets();

  /// Initialize the **jump targets** set, which tells us whether an offset
  /// in the ByteCode stream is a target of some jump command.
  void InitializeJumpTargets();

public:
  LocalContext() = default;
  ~LocalContext() = default;

  /// Initialize both LocalOffsets and JumpTargets.
  void Initialize(const ByteCodeFunction &F);

  /// Return if an offset is a jump target.
  bool IsJumpTarget(unsigned Off) const {
    return JumpTargets.find(Off) != JumpTargets.end();
  }

  /// Return the offset of local name related to frame pointer.
  signed int getLocalOffset(const char *Name) const;

  /// Return whether a name is a variable.
  bool IsVariable(const char *Name) const;

  /// Return whether a name is an array.
  bool IsArray(const char *Name) const;

  /// Return the name of the function being translated.
  const std::string &getFuncName() const;

private:
  std::unordered_map<std::string, signed> LocalOffsets;
  std::unordered_set<unsigned> JumpTargets;
  const ByteCodeFunction *TheFunction = nullptr;
};

} // namespace simplecc
#endif // SIMPLECC_TARGET_LOCALCONTEXT_H
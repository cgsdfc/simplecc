#ifndef LOCAL_CONTEXT_H
#define LOCAL_CONTEXT_H
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace simplecc {
using String = std::string;
class ByteCodeFunction;

// Provide local information for ByteCodeToMipsTranslator
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

  // Return if an offset is a jump target
  bool IsJumpTarget(unsigned Off) const { return JumpTargets.find(Off) != JumpTargets.end(); }

  // Return the offset of local name related to frame pointer
  signed int getLocalOffset(const char *Name) const;

  // Return whether a name is a variable
  bool IsVariable(const char *Name) const;

  // Return whether a name is an array
  bool IsArray(const char *Name) const;

  const String &getName() const;

private:
  std::unordered_map<String, signed> LocalOffsets;
  std::unordered_set<unsigned> JumpTargets;
  const ByteCodeFunction *TheFunction = nullptr;
};

} // namespace simplecc
#endif

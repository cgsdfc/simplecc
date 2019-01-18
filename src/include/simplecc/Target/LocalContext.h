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
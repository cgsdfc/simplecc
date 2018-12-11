#ifndef PASS_H
#define PASS_H

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace simplecompiler {
using String = std::string;
using PassID = const void *;
class Pass;
class PassRegistry;
class PassInfo;
class PassManager;

namespace detail {
template <typename PassT> static PassT *callDefaultCtor() {
  return new PassT();
}
} // namespace detail

/// This class holds metadata about a Pass.
class PassInfo {
  /// The type that creates a Pass instance.
  using PassCtr_t = Pass *(*)();
  /// The name of the Pass. Used as command line argument if non empty.
  const char *Name;
  /// Description for the Pass. Used as help of command line argument.
  const char *Description;
  /// Per class ID of the Pass.
  PassID ID;
  /// Functoin ptr to default construct a Pass.
  PassCtr_t PassCtr;

  /// private ctr. Use PassInfo::Create().
  PassInfo(const char *Name, const char *Description, PassID ID,
           PassCtr_t PassCtr)
      : Name(Name), Description(Description), ID(ID), PassCtr(PassCtr) {}

public:
  PassInfo(PassInfo &&) = delete;
  PassInfo(const PassInfo &) = delete;

  /// Create a PassInfo.
  template <typename PassT>
  static PassInfo *Create(const char *N, const char *D) {
    return new PassInfo(N, D, &PassT::ID,
                        PassCtr_t(detail::callDefaultCtor<PassT>));
  }

  Pass *createPass() {
    assert(PassCtr && "PassCtr must be installed");
    return PassCtr();
  }

  const char *getName() const { return Name; }
  const char *getDescription() const { return Description; }
  PassID getID() const { return ID; }
  void Format(std::ostream &OS) const;
};

inline std::ostream &operator<<(std::ostream &OS, const PassInfo &PI) {
  PI.Format(OS);
  return OS;
}

/// This class holds the registered Pass'es.
class PassRegistry {
  std::map<PassID, std::unique_ptr<PassInfo>> Passes;

public:
  PassRegistry() = default;
  ~PassRegistry() = default;

  PassRegistry(const PassRegistry &) = delete;
  PassRegistry(PassRegistry &&) = delete;

  /// Register a new Pass.
  template <typename PassT>
  void addPass(const char *Name, const char *Description) {
    Passes.emplace(&PassT::ID, PassInfo::Create<PassT>(Name, Description));
  };

  PassInfo *getPassInfo(PassID ID) const {
    assert(Passes.count(ID) && "Unregistered Pass");
    return Passes.find(ID)->second.get();
  }

  template <typename PassT> PassInfo *getPassInfo() const {
    getPassInfo(&PassT::ID);
  }

  PassInfo *getPassInfo(const String &Name) const;
  using const_iterator = decltype(Passes)::const_iterator;
  const_iterator begin() const { return std::begin(Passes); }
  const_iterator end() const { return std::end(Passes); }
  void dump() const;
};

/// Abstract base class for a Pass.
/// A Pass represents a single runnable unit of the compiler.
/// It can run some input data structures, produces some output data
/// structures or write to some stream and a flag about whether it succeeded.
class Pass {
public:
  virtual ~Pass() = default;
  virtual bool run(PassManager &PM) = 0;
};

class PassManager {
  String IFilename;
  String OFilename;
  std::unordered_map<PassID, std::unique_ptr<Pass>> PassCache;
  std::ifstream IFileStream;
  std::ofstream OFileStream;

  Pass *getPassOrCreate(PassID ID);

public:
  PassManager() = default;
  PassManager(PassManager &&) = delete;
  PassManager(const PassManager &) = delete;

  /// Set Input filename to Filename.
  /// \param Filename if it is empty, use stdin.
  /// if it cannot be opened, false is returned and the object
  /// retains old state.
  bool setInputFile(String Filename = "");

  bool setOutputFile(String Filename = "");

  /// Lazily run a Pass and return the result if Pass is OK. Or returns
  /// nullptr if the Pass failed.
  template <typename PassT> PassT *getPass() {
    return static_cast<PassT *>(getPassOrCreate(&PassT::ID));
  }

  /// Lazily run a Pass, which must succeed and retrieve its result.
  template <typename PassT> typename PassT::ResultT getResult() {
    auto P = getPass<PassT>();
    assert(P && "getResult() should never fail");
    return P->getResult();
  }

  /// Return the input stream.
  std::istream &getInputStream();

  /// Return the output stream.
  std::ostream &getOutputStream();

  /// Run a specific pass.
  template <typename PassT> bool run() { return getPass<PassT>() != nullptr; }
  bool run(PassID ID) { return getPassOrCreate(ID) != nullptr; }
};

PassRegistry &getGlobalRegistry();

} // namespace simplecompiler

#endif

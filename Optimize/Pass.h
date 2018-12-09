#ifndef PASS_H
#define PASS_H
#include <fstream>
#include <memory>
#include <unordered_map>
#include <cassert>


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
}

/// This class holds metadata about a Pass.
class PassInfo {
  /// The type that creates a Pass instance.
  using PassCtr_t = Pass *(*)();
  const char *Name;
  const char *Description;
  PassID ID;
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
    return new PassInfo(N, D, &PassT::ID, PassCtr_t(detail::callDefaultCtor<PassT>));
  }

  Pass *createPass() {
    assert(PassCtr && "PassCtr must be installed");
    return PassCtr();
  }

  const char *getName() const { return Name; }
  const char *getDescription() const { return Description; }
  PassID getID() const { return ID; }
};


/// This class holds the registered Pass'es.
class PassRegistry {
  std::unordered_map<PassID, std::unique_ptr<PassInfo>> Passes;

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
  PassManager(PassManager &&) = default;

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

  std::istream &getInputStream();

  std:: ostream &getOutputStream();

};


PassRegistry &getGlobalRegistry();


/// Helper class template to register a Pass.
template <typename PassT> struct RegisterPass {
  RegisterPass(const char *Name, const char *Description) {
    getGlobalRegistry().addPass<PassT>(Name, Description);
  }
};

/// Helper macro of RegisterPass.
#define INITIALIZE_PASS(Class, Name, Description)                              \
  const char Class::ID = 0;                                                    \
  static RegisterPass<Class> Class##Register(Name, Description);

}

#endif

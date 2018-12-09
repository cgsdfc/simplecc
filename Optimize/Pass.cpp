#include "Pass.h"
#include "AST.h"
#include "Assemble.h"
#include "AstBuilder.h"
#include "ByteCodePrinter.h"
#include "Compile.h"
#include "Node.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "SyntaxChecker.h"
#include "Tokenize.h"
#include "TypeChecker.h"
#include <memory>
#include <unordered_map>

/// A Pass represents a single runnable unit of the compiler.
/// It can run some input data structures, produces some output data
/// structures or write to some stream and a flag about whether it succeeded.
/// If the a Pass return a failure flag, the compiler stops immediately.
namespace simplecompiler {
using PassID = const void *;
class Pass;
class PassRegistry;

static PassRegistry &getGlobalRegistry();

class PassManager {
  String Filename;
  std::unordered_map<PassID, std::unique_ptr<Pass>> PassCache;

  Pass *getPassOrCreate(PassID ID);

public:
  PassManager(String Filename = "") : Filename(std::move(Filename)) {}

  /// Lazily run a Pass and return the result if Pass is OK. Or returns
  /// nullptr if the Pass failed.
  template <typename PassT> PassT *getPass();

  /// Lazily run a Pass, which must succeed and retrieve its result.
  template <typename PassT> typename PassT::ResultT getResult() {
    auto P = getPass<PassT>();
    assert(P && "getResult() should never fail");
    return P->getResult();
  }

  template <typename StreamT> StreamT &getInputStream() const {}
  template <typename StreamT> StreamT &getOutputStream() const {}

  /* template <typename PassT> */
  /* bool run() { */
  /*   getResult<PassT>(); */
  /* } */
};

template <typename PassT> static PassT *callDefaultCtor() {
  return new PassT();
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
    return new PassInfo(N, D, &PassT::ID, PassCtr_t(callDefaultCtor<PassT>));
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

/// Helper class template to register a Pass.
template <typename PassT> struct RegisterPass {
  RegisterPass(const char *Name, const char *Description) {
    getGlobalRegistry().addPass<PassT>(Name, Description);
  }
};

/// Abstract base class for a Pass.
class Pass {
public:
  virtual ~Pass() = default;
  virtual bool run(PassManager &PM) = 0;
};

class TokenizePass : public Pass {
  std::vector<TokenInfo> Tokens;

public:
  static const char ID;
  using ResultT = std::vector<TokenInfo>;

  bool run(PassManager &PM) override {
    Tokenize(PM.getInputStream<std::istream>(), Tokens);
    return true;
  }

  ResultT getResult() const { return Tokens; }
};

class ParserPass : public Pass {
  std::unique_ptr<Node> ParseTree;

public:
  static const char ID;
  using ResultT = Node *;

  bool run(PassManager &PM) override {
    auto &&Tokens = PM.getResult<TokenizePass>();
    ParseTree = std::unique_ptr<Node>(ParseTokens(Tokens));
    return bool(ParseTree);
  }

  ResultT getResult() const { return ParseTree.get(); }
};

/// Common base class for Pass'es that process the AST.
class AstPass : public Pass {
  std::unique_ptr<Program> TheAST;

protected:
  void setResult(Program *P) { TheAST.reset(P); }

public:
  AstPass() = default;
  using ResultT = Program *;
  ResultT getResult() { return TheAST.get(); }
};

class BuildAstPass : public AstPass {
public:
  static const char ID;

  bool run(PassManager &PM) override {
    auto PP = PM.getPass<ParserPass>();
    /// ParserPass failed.
    if (!PP)
      return false;
    auto TheProgram = BuildAstFromNode(PP->getResult());
    assert(TheProgram && "BuildAstFromNode() must succeed");
    setResult(TheProgram);
    return true;
  }
};

class SyntaxCheckerPass : public AstPass {
public:
  static const char ID;

  bool run(PassManager &PM) override {
    auto BAP = PM.getPass<BuildAstPass>();
    if (!BAP)
      return false;
    auto TheProgram = BAP->getResult();
    return CheckSyntax(TheProgram);
  }
};

/// This pass builds the symbol table.
class SymbolTablePass : public Pass {
  SymbolTable TheTable;

public:
  using ResultT = const SymbolTable &;
  static const char ID;
  bool run(PassManager &PM) override {
    auto SCP = PM.getPass<SyntaxCheckerPass>();
    if (!SCP)
      return false;
    return TheTable.Build(SCP->getResult());
  }

  SymbolTable &getResult() { return TheTable; }
  const SymbolTable &getResult() const { return TheTable; }
};

class TypeCheckerPass : public AstPass {
public:
  static const char ID;

  bool run(PassManager &PM) override {
    auto STP = PM.getPass<SymbolTablePass>();
    if (!STP)
      return false;
    /// The SymbolTablePass's success implies that of BuildAstPass.
    auto TheProgram = PM.getResult<BuildAstPass>();
    return CheckType(TheProgram, STP->getResult());
  }
};

/// This is the pass that every backend pass should depened on.
class AnalysisPass : public TypeCheckerPass {
public:
  static const char ID;
  using TypeCheckerPass::run;
};

class PrintByteCodePass : public Pass {
public:
  static const char ID;
  bool run(PassManager &PM) override {
    auto AP = PM.getPass<AnalysisPass>();
    if (!AP)
      return false;
    PrintByteCode(AP->getResult(), PM.getOutputStream<std::ostream>());
    return true;
  }
};

class CompilePass : public Pass {
  CompiledModule TheModule;

public:
  static const char ID;
  using ResultT = const CompiledModule &;
  ResultT getResult() const { return TheModule; }

  bool run(PassManager &PM) override {
    auto AP = PM.getPass<AnalysisPass>();
    if (!AP)
      return false;
    TheModule =
        CompileProgram(AP->getResult(), PM.getResult<SymbolTablePass>());
    return true;
  }
};

class AssemblePass : public Pass {
public:
  static const char ID;
  bool run(PassManager &PM) override {
    const CompiledModule &CM = PM.getResult<CompilePass>();
    AssembleMips(CM, PM.getOutputStream<std::ostream>());
    return true;
  }
};

Pass *PassManager::getPassOrCreate(PassID ID) {
  auto I = PassCache.find(ID);
  if (I != PassCache.end())
    return I->second.get();
  PassInfo *PI = getGlobalRegistry().getPassInfo(ID);
  std::unique_ptr<Pass> P(PI->createPass());
  bool OK = P->run(*this);
  if (!OK)
    return nullptr;
  auto Result = PassCache.emplace(ID, std::move(P));
  assert(Result.second);
  return Result.first->second.get();
}

template <typename PassT> PassT *PassManager::getPass() {
  auto P = getPassOrCreate(&PassT::ID);
  return static_cast<PassT *>(P);
}

static PassRegistry ThePassRegistry;

static PassRegistry &getGlobalRegistry() { return ThePassRegistry; }

#define INITIALIZE_PASS(Class, Name, Description)                              \
  const char Class::ID = 0;                                                    \
  static RegisterPass<Class> Class##Register(Name, Description);

INITIALIZE_PASS(TokenizePass, "tokenize", "break input into tokens")
INITIALIZE_PASS(ParserPass, "parse", "parse tokens")
INITIALIZE_PASS(BuildAstPass, "build-ast",
                "create an abstract syntax tree from the concrete syntax tree")
INITIALIZE_PASS(SyntaxCheckerPass, "syntax-check",
                "verify that the AST is syntactically correct")
INITIALIZE_PASS(SymbolTablePass, "symbol-table",
                "build a symbol table from the abstract syntax tree")
INITIALIZE_PASS(TypeCheckerPass, "type-check",
                "run type-check for the input program")
INITIALIZE_PASS(AnalysisPass, "", "")

INITIALIZE_PASS(PrintByteCodePass, "print-bytecode",
                "print byte code in quarternary form")
INITIALIZE_PASS(CompilePass, "compile",
                "compile the input program to byte code")
INITIALIZE_PASS(AssemblePass, "assemble",
                "assemble the compiled program to MIPS assembly")
} // namespace simplecompiler

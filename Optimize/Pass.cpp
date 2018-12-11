#include "Pass.h"
#include "AST.h"
#include "Assemble.h"
#include "AstBuilder.h"
#include "ByteCodePrinter.h"
#include "Compile.h"
#include "Node.h"
#include "Parser.h"
#include "Print.h"
#include "SymbolTable.h"
#include "SyntaxChecker.h"
#include "Tokenize.h"
#include "TypeChecker.h"

#ifdef SIMPLE_COMPILER_USE_LLVM
#include "EmitLLVM.h"
#endif

#include <algorithm>
#include <cstring>

namespace simplecompiler {

/// Helper class template to register a Pass.
template <typename PassT> struct RegisterPass {
  RegisterPass(const char *Name, const char *Description) {
    getGlobalRegistry().addPass<PassT>(Name, Description);
  }
};

/// Helper macro of RegisterPass.
#define INITIALIZE_PASS(Class, Name, Description)                              \
  char Class::ID = 0;                                                          \
  static RegisterPass<Class> Class##Register(Name, Description);

std::istream &PassManager::getInputStream() {
  return InputFilename.empty() ? std::cin : InputFileStream;
}

std::ostream &PassManager::getOutputStream() {
  return OutputFilename.empty() ? std::cout : OutputFileStream;
}

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

/// Set Input filename to Filename.
/// \param Filename if it is empty, use stdin.
/// if it cannot be opened, false is returned and the object
/// retains old state.
bool PassManager::setInputFile(String Filename) {
  // Set to stdin.
  if (Filename.empty()) {
    InputFilename.clear();
    return true;
  }
  std::ifstream File(Filename);
  if (File.fail())
    return false;
  InputFileStream = std::move(File);
  InputFilename = std::move(Filename);
  return true;
}

bool PassManager::setOutputFile(String Filename) {
  if (Filename.empty()) {
    OutputFilename.clear();
    return true;
  }
  std::ofstream File(Filename);
  if (File.fail())
    return false;
  OutputFileStream = std::move(File);
  OutputFilename = std::move(Filename);
  return true;
}

PassInfo *PassRegistry::getPassInfo(const String &Name) const {
  auto Iter = std::find_if(Passes.begin(), Passes.end(),
                           [Name](decltype(Passes)::const_reference Pair) {
                             return Name == Pair.second->getName();
                           });
  assert(Iter != Passes.end() && "Unregistered PassInfo");
  return Iter->second.get();
}

class TokenizePass : public Pass {
  std::vector<TokenInfo> Tokens;

public:
  static char ID;
  using ResultT = const std::vector<TokenInfo> &;

  bool run(PassManager &PM) override {
    Tokenize(PM.getInputStream(), Tokens);
    return true;
  }

  ResultT getResult() const { return Tokens; }
};
INITIALIZE_PASS(TokenizePass, "", "")

class ParserPass : public Pass {
  std::unique_ptr<Node> ParseTree;

public:
  static char ID;
  using ResultT = Node *;

  bool run(PassManager &PM) override {
    auto &&Tokens = PM.getResult<TokenizePass>();
    ParseTree = std::unique_ptr<Node>(ParseTokens(Tokens));
    return ParseTree != nullptr;
  }

  ResultT getResult() const { return ParseTree.get(); }
};

INITIALIZE_PASS(ParserPass, "", "")

template <typename ResultT> struct PrinterTrait {
  static void Print(std::ostream &OS, ResultT R) { OS << R << "\n"; }
};

/// Generic base class that prints out the result of some pass.
/// Derived class can provide their own Print() or uses the default impl here.
template <typename Base> class PrintPassMixin : public Base {
  using TraitT = PrinterTrait<typename Base::ResultT>;

public:
  static char ID;
  bool run(PassManager &PM) override {
    if (Base::run(PM)) {
      TraitT::Print(PM.getOutputStream(), Base::getResult());
      return true;
    }
    return false;
  }
};

template <> struct PrinterTrait<TokenizePass::ResultT> {
  static void Print(std::ostream &OS, TokenizePass::ResultT R) {
    PrintTokens(R, OS);
  }
};

template <typename T> struct PrinterTrait<T *> {
  static void Print(std::ostream &OS, T *R) { OS << *R << "\n"; }
};

class BuildAstPass : public Pass {
  std::unique_ptr<Program> TheProgram;

public:
  static char ID;
  using ResultT = Program *;

  bool run(PassManager &PM) override {
    auto PP = PM.getPass<ParserPass>();
    /// ParserPass failed.
    if (!PP)
      return false;
    TheProgram.reset(BuildAstFromNode(PP->getResult()));
    return true;
  }

  Program *getResult() const { return TheProgram.get(); }
};

INITIALIZE_PASS(BuildAstPass, "", "")

using PrintTokensPass = PrintPassMixin<TokenizePass>;
static RegisterPass<PrintTokensPass> PrintTokensPassRegister("print-tokens",
                                                             "");
template <> char PrintTokensPass::ID = 0;

using PrintNodePass = PrintPassMixin<ParserPass>;
static RegisterPass<PrintNodePass>
    PrintNodePassRegister("print-nodes", "dump nodes of concrete syntax tree");
template <> char PrintNodePass::ID = 0;

using PrintAstPass = PrintPassMixin<BuildAstPass>;
static RegisterPass<PrintAstPass>
    PrintAstPassRegister("print-ast", "dump the nodes of abstract syntax tree");
template <> char PrintAstPass::ID = 0;

class SyntaxCheckerPass : public Pass {
  Program *TheProgram;

public:
  static char ID;

  bool run(PassManager &PM) override {
    auto BAP = PM.getPass<BuildAstPass>();
    if (!BAP)
      return false;
    TheProgram = BAP->getResult();
    return CheckSyntax(TheProgram);
  }

  Program *getResult() const { return TheProgram; }
};

INITIALIZE_PASS(SyntaxCheckerPass, "syntax-check",
                "verify that the AST is syntactically correct")

/// This pass builds the symbol table.
class SymbolTablePass : public Pass {
  SymbolTable TheTable;

public:
  using ResultT = const SymbolTable &;
  static char ID;
  bool run(PassManager &PM) override {
    auto SCP = PM.getPass<SyntaxCheckerPass>();
    if (!SCP)
      return false;
    return TheTable.Build(SCP->getResult());
  }

  SymbolTable &getResult() { return TheTable; }
  const SymbolTable &getResult() const { return TheTable; }
};

INITIALIZE_PASS(SymbolTablePass, "", "")

using PrintSymbolTablePass = PrintPassMixin<SymbolTablePass>;
static RegisterPass<PrintSymbolTablePass>
    PrintSymbolTablePassRegister("print-symbol-table", "");
template <> char PrintSymbolTablePass::ID = 0;

class TypeCheckerPass : public Pass {
  Program *TheProgram;

public:
  static char ID;

  bool run(PassManager &PM) override {
    auto STP = PM.getPass<SymbolTablePass>();
    if (!STP)
      return false;
    /// The SymbolTablePass's success implies that of BuildAstPass.
    TheProgram = PM.getResult<BuildAstPass>();
    return CheckType(TheProgram, STP->getResult());
  }

  Program *getResult() const { return TheProgram; }
};

INITIALIZE_PASS(TypeCheckerPass, "type-check",
                "run type-check for the input program")

/// This is the pass that every backend pass should depened on.
class AnalysisPass : public TypeCheckerPass {
public:
  static char ID;
  using TypeCheckerPass::run;
};

INITIALIZE_PASS(AnalysisPass, "", "")

class PrintByteCodePass : public Pass {
public:
  static char ID;
  bool run(PassManager &PM) override {
    auto AP = PM.getPass<AnalysisPass>();
    if (!AP)
      return false;
    PrintByteCode(AP->getResult(), PM.getOutputStream());
    return true;
  }
};

INITIALIZE_PASS(PrintByteCodePass, "print-bytecode",
                "print byte code in quarternary form")

class CompilePass : public Pass {
  CompiledModule TheModule;

public:
  static char ID;
  using ResultT = const CompiledModule &;
  ResultT getResult() const { return TheModule; }

  bool run(PassManager &PM) override {
    auto AP = PM.getPass<AnalysisPass>();
    if (!AP)
      return false;
    TheModule.Build(AP->getResult(), PM.getResult<SymbolTablePass>());
    return true;
  }
};

using PrintCompilePass = PrintPassMixin<CompilePass>;
static RegisterPass<PrintCompilePass>
    PrintCompilePassRegistry("compile",
                             "compile the input program to byte code");
template <> char PrintCompilePass::ID = 0;

INITIALIZE_PASS(CompilePass, "", "")

class AssemblePass : public Pass {
public:
  static char ID;
  bool run(PassManager &PM) override {
    const CompiledModule &CM = PM.getResult<CompilePass>();
    AssembleMips(CM, PM.getOutputStream());
    return true;
  }
};

INITIALIZE_PASS(AssemblePass, "assemble",
                "assemble the compiled program to MIPS assembly")

#ifdef SIMPLE_COMPILER_USE_LLVM
class EmitLLVMPass : public Pass {
public:
  static char ID;
  EmitLLVMPass() = default;
  bool run(PassManager &PM) override {
    auto AP = PM.getPass<AnalysisPass>();
    if (!AP) return false;
    return CompileToLLVMIR(PM.getInputFilename(),
                           AP->getResult(),
                           PM.getResult<SymbolTablePass>(),
                           PM.getOutputFilename());

  }
};

INITIALIZE_PASS(EmitLLVMPass, "emit-llvm", "emit LLVM IR")
#endif

/// If we don't put that onto the heap, it breaks at runtime...
/// See llvm::ManagedStatic<>.
static std::unique_ptr<PassRegistry> ThePassRegistry;

PassRegistry &getGlobalRegistry() {
  if (!ThePassRegistry) {
    ThePassRegistry = std::make_unique<PassRegistry>();
  }
  return *ThePassRegistry;
}

void PassInfo::Format(std::ostream &OS) const {
  OS << "PassInfo(ID=" << ID << ", "
     << "Name=" << Name;
  OS << ", "
     << "Description=" << Description << ")";
}

void PassRegistry::dump() const {
  for (auto &&Pair : Passes) {
    PrintErrs(*Pair.second);
  }
}

} // namespace simplecompiler

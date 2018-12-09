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

namespace simplecompiler {

std::istream &PassManager::getInputStream() {
  return IFilename.empty() ? std::cin : IFileStream;
}

std::ostream &PassManager::getOutputStream() {
  return OFilename.empty() ? std::cout : OFileStream;
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
    IFilename.clear();
    return true;
  }
  std::ifstream File(Filename);
  if (File.fail())
    return false;
  IFileStream = std::move(File);
  IFilename = std::move(Filename);
  return true;
}

bool PassManager::setOutputFile(String Filename) {
  if (Filename.empty()) {
    OFilename.clear();
    return true;
  }
  std::ofstream File(Filename);
  if (File.fail())
    return false;
  OFileStream = std::move(File);
  OFilename = std::move(Filename);
  return true;
}

class TokenizePass : public Pass {
  std::vector<TokenInfo> Tokens;

public:
  static const char ID;
  using ResultT = std::vector<TokenInfo>;

  bool run(PassManager &PM) override {
    Tokenize(PM.getInputStream(), Tokens);
    return true;
  }

  ResultT getResult() const { return Tokens; }
};
INITIALIZE_PASS(TokenizePass, "tokenize", "break input into tokens")

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

INITIALIZE_PASS(ParserPass, "parse", "parse tokens")

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

INITIALIZE_PASS(BuildAstPass, "build-ast",
                "create an abstract syntax tree from the concrete syntax tree")

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

INITIALIZE_PASS(SyntaxCheckerPass, "syntax-check",
                "verify that the AST is syntactically correct")

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

INITIALIZE_PASS(SymbolTablePass, "symbol-table",
                "build a symbol table from the abstract syntax tree")

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

INITIALIZE_PASS(TypeCheckerPass, "type-check",
                "run type-check for the input program")

/// This is the pass that every backend pass should depened on.
class AnalysisPass : public TypeCheckerPass {
public:
  static const char ID;
  using TypeCheckerPass::run;
};

INITIALIZE_PASS(AnalysisPass, "", "")

class PrintByteCodePass : public Pass {
public:
  static const char ID;
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

INITIALIZE_PASS(CompilePass, "compile",
                "compile the input program to byte code")
class AssemblePass : public Pass {
public:
  static const char ID;
  bool run(PassManager &PM) override {
    const CompiledModule &CM = PM.getResult<CompilePass>();
    AssembleMips(CM, PM.getOutputStream());
    return true;
  }
};

INITIALIZE_PASS(AssemblePass, "assemble",
                "assemble the compiled program to MIPS assembly")

static PassRegistry ThePassRegistry;

PassRegistry &getGlobalRegistry() { return ThePassRegistry; }

} // namespace simplecompiler

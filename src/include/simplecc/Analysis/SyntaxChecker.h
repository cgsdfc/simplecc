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

#ifndef SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#define SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
#include "simplecc/AST/ChildrenVisitor.h"
#include "simplecc/Support/ErrorManager.h"

namespace simplecc {
/// @brief SyntaxChecker performs syntax checks on the program.
/// Since the Parser is based on a rewritten grammar, it loosens the restrictions
/// of the original one. Malformed code can be accepted by the Parser, so this class
/// rejects them.
/// The syntax checks includes:
/// 1. The order of global declarations should be: ``ConstDecl VarDecl FuncDef``.
/// 2. Size of Array cannot be zero.
/// 3. Variable types cannot be void.
/// 4. Type of a ConstDecl must match its value.
/// 5. The last declaration of a program must be the main function with a proper signature.
class SyntaxChecker : VisitorBase<SyntaxChecker> {
  void visitProgram(ProgramAST *P);
  void visitConstDecl(ConstDecl *CD);
  void visitVarDecl(VarDecl *VD);
  void visitArgDecl(ArgDecl *AD);
  void visitFuncDef(FuncDef *FD);
  /// Return if a decl is void main().
  bool isMainFunction(DeclAST *D) const;

public:
  SyntaxChecker();
  /// Perform syntax check on the program.
  /// Return true is the program is ill-formed.
  bool Check(ProgramAST *P);

private:
  friend VisitorBase;
  ErrorManager EM;
};
} // namespace simplecc
#endif // SIMPLECC_ANALYSIS_SYNTAXCHECKER_H
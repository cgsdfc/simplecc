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

/// @file External interface of the Visualize module.
/// This module can visualize an AST or CST in the dot format, which can be rendered
/// in different image formats.
#ifndef SIMPLECC_VISUALIZE_VISUALIZE_H
#define SIMPLECC_VISUALIZE_VISUALIZE_H
#include <llvm/Support/raw_ostream.h>
#include <iostream>

namespace simplecc {
class ProgramAST;
class Node;
class AST;

/// This function prints a short description for each AST node in a program.
void PrintAllASTNodes(ProgramAST *P, std::ostream &O);

/// This function writes a DOT graph of an AST to output stream.
void WriteASTGraph(ProgramAST *P, llvm::raw_ostream &O);

/// This function prints a short description for each CST node in a program.
void PrintAllNodes(Node *Root);

/// This function write a DOT graph of a CST to output stream.
void WriteCSTGraph(Node *Root, llvm::raw_ostream &O);
} // namespace simplecc
#endif // SIMPLECC_VISUALIZE_VISUALIZE_H
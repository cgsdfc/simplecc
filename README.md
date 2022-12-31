# A simple and modular C-like compiler in C++

## Introduction

This project uses modern C++ to achieve the same goal. This project highlights a lot of improvement over the old version, which includes a pure object-oriented implementation, well-formatted code (LLVM style), better tests and documentation, better source code organization, and a richer feature set.


## Source language

The language this compiler compiles is a tiny subset of C, which is called ``C0``. The formal grammar of it can be found in the ``Grammar`` file located at the project root, which is written in an extended BNF, in the same format as the grammar specification of Python. In brief, it consists of three kinds of declarations, namely _constant declaration_, _variable declaration_ and _function definition_ and a subset of C's varieties of statement and expression. To see some examples, visit the ``Tests/`` at the project root, where files with ``.c`` suffix under ``src/`` folders are examples of this language.


## Features

The main feature of simplecc is compiling the source down to MIPS assembly, which can be executed by the
[Mars simulator](https://courses.missouristate.edu/KenVollmar/MARS). The command is:
```
simplecc --asm <source>
```
Assuming you already know Mars, use this command to launch it in a command line to interpret the MIPS code:
```
java -jar Mars_<version>.jar <asm-file>
```
For more usages about the Mars simulator, please refer to their documentation.
Besides emitting MIPS code, simplecc can emit [LLVM IR](https://llvm.org/docs/LangRef.html) as well.
The command is:
```
simplecc --emit-llvm <source>
```
With that, a native executable can be derived by:
```
simplecc --emit-llvm | clang -x ir
```
which generates an ``a.out`` in the current directory. Other than these, simplecc comes with some nice features to inspect the intermediate
results of the compiler. To see a stream of tokens, run:
```
simplecc --print-tokens <source>
```
This will print the tokens extracted from the source with each one in a line.
The command
```
simplecc --print-ast <source>
```
will pretty-print the AST representing the program with proper indentations. If built with llvm libraries, the command
```
simplecc --ast-graph <source>
```
will generate a dot format for visualizing the AST, which can be passed to ``dot`` to obtain an image, as in:
```
simplecc --ast-graph <source> | dot -T<format> -o <output>
```
If you just want to check the syntax of a source file, simplecc provides an option similar to ``-fsyntax-only``:
```
simplecc --check-only <source>
```
This will report any detected problem to stderr and return a proper error code. All options will write the output to stdout by default, or you can
specify a filename through the ``-o`` switch to store it elsewhere. For input, if no source file is given, it will read from stdin. Thus, simplecc allows
to used as a filter:
```
cat <source> | simplecc --emit-llvm | clang -x ir -o <output>
```
For options undocumented here, you can use
```
simplecc --help
```
to see all available sub-commands.

## Build and Install

To build simplecc, all you need is a C++ compiler that supports C++ 11 standard and cmake. The process is straightforward:
```
cd simplecc/
mkdir build/
cd build/
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME ../src
make && make install
```
The executable ``simplecc`` is the only artifact that got installed.
If llvm libraries are installed, it will be detected and simplecc will gain the ability to emit LLVM IR and visualize
various tree structures in the front end. In other words, switches ``--emit-llvm``, ``--cst-graph`` and ``--ast-graph``
will be available.


## Optimization and Benchmark

The compiler **does not** implement any heavy optimization itself. It just performs the basic constant folding and removal of
unreachable code. However, with the powerful llvm infrastructure, you can perform meaningful optimizations. Reference commands:
```
simplecc --emit-llvm <source> | opt - -S
```
This will print to stderr the optimized LLVM bitcode in human-readable form. Without the ``-S``, you get the machine form.
After that, use ``clang`` to get a native executable:
```
clang -x ir <bitcode> -o <output>
```
The ``Benchmark/Sample4Opt-1-while.c`` can be used to benchmark the compiler.

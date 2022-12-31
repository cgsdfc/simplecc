# A simple and modular C-like compiler in C++

## 1. Source language

This project is a small compiler written in modern C++ for education and practice. The source language it compiles is called C0, which is a modified subset of C for educational purposes. The BNF grammar rules of C0 can be found in the file ``Grammar``. In brief, this tiny language consists of the following components:

1. Constant declarations, like `const int a = 1, f = 2;`
2. Variable declarations, like `char b; int c;`
3. Function definitions, like `void func() {}`
4. Some C-like statements and expressions. 

The ``test/`` folder contains some vivid examples of the C0 language (files with ``.c0`` suffix).


## 2. Supported backends

### 2.1 Mips Assembly

We have a hand-rolled stack-based IR generator that emits unoptimized Mips assembly, which can be executed by the [Mars simulator](https://courses.missouristate.edu/KenVollmar/MARS). To use this backend, please run:
```
simplecc --asm input.c0
```
For more examples of the emitted Mips assembly

### 2.2 LLVM IR

We also integrate with the LLVM backend to enjoy its capacities of optimization and native codegen. To obtain an LLVM IR file, please run:
```
simplecc --emit-llvm input.c0
```
For more examples of emitted IR, please see the `*.ll` files in `test/LLVM/ir/`.

To obtain a native executable, please run:
```
simplecc --emit-llvm | clang -x ir
```


## 3. Visualization & debug support

We provide some support to inspect the intermediate results of the compiler. 

### 3.1 Tokens
To see a stream of tokens, please run:
```
simplecc --print-tokens input.c0
```
This will print details about the tokens extracted from the source (one token in a line).

### 3.2 AST

To pretty-print the AST of the program, please run:
```
simplecc --print-ast input.c0
```

To obtain a flowchart of the AST, a graphviz-based diagram of AST can be generated with:
```
simplecc --ast-graph input.c0
```
This command will generate a dot file, which can be passed to the command ``dot`` as follows:
```
simplecc --ast-graph input.c0 | dot -Tpng -o input.png
```


### 3.3 Syntax checking

To check the syntax of a source file (but not emitting anything), please run:
```
simplecc --check-only input.c0
```
This will print any detected errors to the console.


## 4. Build & Install

This project requires a modern compiler that supports C++11. Please run the following commands to build and install our executable:
```
cd simplecc/ && mkdir -p build/
cd build/ && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME ../src
make && make install
```
The command `simplecc` will then be available.


## 5. Optimization

For simplicity, we do not implement too many optimization passes. Only simple constant folding and dead code elimination are implemented. Please see `src/lib/Transform` for more details.


## 6. Citation

If you use our code in your research, we highly recommend your proper citation as follows:

```bibtex
@misc{cong_simplecc_2018,
	title = {Simplecc: {A} simple and modular {C}-like compiler in {C}++},
	shorttitle = {Simplecc},
	url = {https://gitee.com/cgsdfc/simplecc.git},
	abstract = {Simplecc is a small compiler written in modern C++ that compiles an educational C-like language C0. It has a primary backend that emits stack-based unoptimized Mips assembly. It also integrates with an external LLVM backend that enables optimization and native codegen. Internally it uses a generated parser and generated abstract syntax tree with other important components written in an object-oriented style.},
	author = {Cong Feng},
	year = {2018},
}
```
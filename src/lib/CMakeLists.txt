set(EXEC_NAME simplecc)

set(SOURCE_FILES
        # First thing first, tokenize.
        Lex/Tokenize.cpp

        # Turn tokens into CST, then into AST.
        Parse/AST.cpp
        Parse/Grammar.cpp
        Parse/AstBuilder.cpp
        Parse/Parser.cpp
        Parse/gramdef.cpp
        Parse/Node.cpp
        Parse/Parse.cpp

        # Perform various analyses (or checks) on the AST.
        Analysis/TypeChecker.cpp
        Analysis/SymbolTable.cpp
        Analysis/SyntaxChecker.cpp
        Analysis/Types.cpp
        Analysis/ImplicitCallTransformer.cpp
        Analysis/AstVerifier.cpp
        Analysis/SymbolTableBuilder.cpp
        Analysis/AnalysisManager.cpp

        # Generate ByteCode from the AST.
        Codegen/ByteCodePrinter.cpp
        Codegen/ByteCode.cpp
        Codegen/ByteCodeModule.cpp
        Codegen/ByteCodeBuilder.cpp
        Codegen/ByteCodeFunction.cpp
        Codegen/Opcode.cpp
        Codegen/ByteCodeCompiler.cpp

        # Turn the ByteCode into MIPS assembly.
        Target/Assemble.cpp
        Target/LocalContext.cpp
        Target/ByteCodeToMipsTranslator.cpp
        Target/MipsSupport.cpp
        Target/MipsAssemblyWriter.cpp

        Driver/Driver.cpp

        # Optimization is not implemented currently.
        # That's why they come last.
        Optimize/Module.cpp
        Optimize/Function.cpp
        Optimize/Instruction.cpp
        Optimize/BasicBlock.cpp
        Driver/Driver.cpp)

set(EXTRA_LIBRARIES "")

if (USE_LLVM)
    list(APPEND SOURCE_FILES
            # Visualize the CST.
            Visualize/CSTGraph.cpp
            Visualize/NodeIterator.cpp

            # Visualize the AST.
            Visualize/ASTGraph.cpp
            Visualize/AstRef.cpp
            Visualize/AstIterator.cpp
            Visualize/DescriptionVisitor.cpp
            Visualize/ChildrenCollector.cpp

            # Generate LLVM IR from the AST.
            LLVM/LLVMTypeMap.cpp
            LLVM/LLVMValueMap.cpp
            LLVM/LLVMIRCompiler.cpp
            LLVM/EmitLLVM.cpp

            )
    list(APPEND EXTRA_LIBRARIES ${llvm_libs})
    add_definitions(-DSIMPLE_COMPILER_USE_LLVM)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
endif (USE_LLVM)

if (MSVC)
    list(APPEND SOURCE_FILES Driver/windows_main.cpp)
else ()
    list(APPEND SOURCE_FILES Driver/CommandLine.cpp)
    list(APPEND SOURCE_FILES Driver/main.cpp)
endif ()

add_executable(${EXEC_NAME} ${SOURCE_FILES})
target_link_libraries(${EXEC_NAME} ${EXTRA_LIBRARIES})

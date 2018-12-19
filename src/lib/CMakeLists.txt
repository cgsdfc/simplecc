set(EXEC_NAME simplecc)

set(SOURCE_FILES
        Analysis/ByteCodePrinter.cpp
        Codegen/Assemble.cpp
        Codegen/ByteCode.cpp
        Codegen/ByteCodeModule.cpp
        Codegen/Opcode.cpp
        Parse/AST.cpp
        Parse/Grammar.cpp
        Analysis/TypeChecker.cpp
        Analysis/SymbolTable.cpp
        Analysis/SyntaxChecker.cpp
        Parse/AstBuilder.cpp
        Parse/Parser.cpp
        Parse/gramdef.cpp
        Parse/Node.cpp
        Support/Pass.cpp
        Optimize/Module.cpp
        Analysis/Types.cpp
        Analysis/ImplicitCallTransformer.cpp
        Analysis/AstVerifier.cpp
        Codegen/ByteCodeBuilder.cpp
        Codegen/ByteCodeFunction.cpp
        Codegen/LocalContext.cpp
        Codegen/ByteCodeToMipsTranslator.cpp
        Codegen/MipsSupport.cpp
        Codegen/MipsAssemblyWriter.cpp
        Lex/Tokenize.cpp
        Analysis/SymbolTableBuilder.cpp
        Codegen/ByteCodeCompiler.cpp)

set(EXTRA_LIBRARIES "")

if (USE_LLVM)
    list(APPEND SOURCE_FILES
            Visualize/CSTGraph.cpp
            Visualize/ASTGraph.cpp
            Visualize/AstRef.cpp
            Visualize/AstIterator.cpp
            Visualize/DescriptionVisitor.cpp
            Visualize/NodeIterator.cpp
            Visualize/ChildrenCollector.cpp
            LLVM/LLVMTypeMap.cpp
            LLVM/LLVMValueMap.cpp
            LLVM/LLVMIRCompiler.cpp
            LLVM/EmitLLVM.cpp)
    list(APPEND EXTRA_LIBRARIES ${llvm_libs})
    add_definitions(-DSIMPLE_COMPILER_USE_LLVM)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
endif (USE_LLVM)

if (MSVC)
    list(APPEND SOURCE_FILES Driver/windows_main.cpp)
else ()
    list(APPEND SOURCE_FILES Driver/main.cpp)
endif ()

add_executable(${EXEC_NAME} ${SOURCE_FILES})
target_link_libraries(${EXEC_NAME} ${EXTRA_LIBRARIES})

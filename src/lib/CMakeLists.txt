set(EXEC_NAME simplecc)

set(SOURCE_FILES
  ./ByteCodePrinter.cpp
  ./Assemble.cpp
  ./ByteCode.cpp
  ./ByteCodeModule.cpp
  ./Opcode.cpp
  ./AST.cpp
  ./Grammar.cpp
  ./TypeChecker.cpp
  ./SymbolTable.cpp
  ./SyntaxChecker.cpp
  ./AstBuilder.cpp
  ./Parser.cpp
  ./gramdef.cpp
  ./Node.cpp
  ./Pass.cpp
  ./Module.cpp
  ./Types.cpp
  ./ImplicitCallTransformer.cpp
  ./AstVerifier.cpp
  ./ByteCodeBuilder.cpp
  ./ByteCodeFunction.cpp
  ./LocalContext.cpp
  ./ByteCodeToMipsTranslator.cpp
  ./MipsSupport.cpp
  ./MipsAssemblyWriter.cpp
  ./Tokenize.cpp
  ./SymbolTableBuilder.cpp
  ./ByteCodeCompiler.cpp)

set(EXTRA_LIBRARIES "")

if (USE_LLVM)
    list(APPEND SOURCE_FILES
            ./CSTGraph.cpp
            ./ASTGraph.cpp
            ./AstRef.cpp
            ./EmitLLVM.cpp)
    list(APPEND EXTRA_LIBRARIES ${llvm_libs})
    add_definitions(-DSIMPLE_COMPILER_USE_LLVM)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
endif (USE_LLVM)

if (MSVC)
    list(APPEND SOURCE_FILES ./windows_main.cpp)
else ()
    list(APPEND SOURCE_FILES ./main.cpp)
endif ()

add_executable(${EXEC_NAME} ${SOURCE_FILES})
target_link_libraries(${EXEC_NAME} ${EXTRA_LIBRARIES})

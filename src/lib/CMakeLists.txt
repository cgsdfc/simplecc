set(EXEC_NAME simplecc)

set(SOURCE_FILES
        # First thing first, tokenize.
        Lex/Tokenize.cpp

        # Turn tokens into CST, then into AST.
        Parse/AST.cpp
        Parse/Grammar.cpp
        Parse/ASTBuilder.cpp
        Parse/Parser.cpp
        Parse/GramDef.cpp
        Parse/Node.cpp
        Parse/Parse.cpp
        Parse/Enums.cpp
        Parse/ParseTreePrinter.cpp

        # Perform various analyses (or checks) on the AST.
        Analysis/TypeChecker.cpp
        Analysis/SymbolTable.cpp
        Analysis/SyntaxChecker.cpp
        Analysis/Types.cpp
        Analysis/ImplicitCallTransformer.cpp
        Analysis/ASTVerifier.cpp
        Analysis/SymbolTableBuilder.cpp
        Analysis/AnalysisManager.cpp
        Analysis/ArrayBoundChecker.cpp
        Analysis/ASTPrettyPrinter.cpp

        # Generate ByteCode from the AST.
        CodeGen/ByteCodePrinter.cpp
        CodeGen/ByteCode.cpp
        CodeGen/ByteCodeModule.cpp
        CodeGen/ByteCodeBuilder.cpp
        CodeGen/ByteCodeFunction.cpp
        CodeGen/ByteCodeCompiler.cpp
        CodeGen/Compile.cpp

        # Turn the ByteCode into MIPS assembly.
        Target/Assemble.cpp
        Target/LocalContext.cpp
        Target/ByteCodeToMipsTranslator.cpp
        Target/MipsSupport.cpp
        Target/MipsAssemblyWriter.cpp

        # TODO: descript IR.
        IR/Module.cpp
        IR/Function.cpp
        IR/Instruction.cpp
        IR/Instructions.cpp
        IR/BasicBlock.cpp
        IR/Use.cpp
        IR/IRBuilder.cpp
        IR/Type.cpp
        IR/Argument.cpp
        IR/Value.cpp
        IR/GlobalVariable.cpp
        IR/User.cpp
        IR/Constant.cpp
        IR/IRContext.cpp
        IR/IRCompiler.cpp
        IR/IRPrinter.cpp

        Transform/TrivialConstantFolder.cpp
        Transform/DeadCodeEliminator.cpp
        Transform/Transform.cpp

        Driver/DriverBase.cpp
        # WindowsDriver is always built for the sake of IDE.
        Driver/WindowsDriver.cpp
        Driver/main.cpp)

if (NOT MSVC)
    list(APPEND SOURCE_FILES Driver/Driver.cpp)
endif ()

set(EXTRA_LIBRARIES "")

if (USE_LLVM)
    list(APPEND SOURCE_FILES
            # Visualize the CST.
            Visualize/CSTGraph.cpp
            # Visualize the AST.
            Visualize/ASTGraph.cpp
            Visualize/DescriptionVisitor.cpp

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

add_executable(${EXEC_NAME} ${SOURCE_FILES})
target_link_libraries(${EXEC_NAME} ${EXTRA_LIBRARIES})
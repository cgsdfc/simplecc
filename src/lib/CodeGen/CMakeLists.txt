add_library(CodeGen STATIC
        ByteCode.cpp
        ByteCodeBuilder.cpp
        ByteCodeCompiler.cpp
        ByteCodeFunction.cpp
        ByteCodeModule.cpp
        ByteCodePrinter.cpp
        CodeGen.cpp)

target_link_libraries(CodeGen Analysis)
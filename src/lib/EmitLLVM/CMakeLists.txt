add_library(EmitLLVM STATIC
        LLVM.cpp
        LLVMIRCompiler.cpp
        LLVMTypeMap.cpp
        LLVMValueMap.cpp)

target_link_libraries(EmitLLVM Analysis)
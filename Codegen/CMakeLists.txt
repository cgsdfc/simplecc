project(Codegen)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

# Find TCLAP comand line library
if (NOT WIN32)
  find_path(TCLAP_INCLUDE
    NAMES tclap/CmdLine.h
    PATHS "$ENV{HOME}/include"
    DOC "Tclap command line library's headers")

  message(${TCLAP_INCLUDE})
  include_directories(${TCLAP_INCLUDE})
  set(MAIN_CPP "main.cpp")
else (NOT WIN32)
  set(MAIN_CPP "windows_main.cpp")
endif (NOT WIN32)

set(USE_LLVM OFF)

# Find LLVM infrustrament
if (NOT WIN32 AND ${USE_LLVM})
  find_package(LLVM REQUIRED CONFIG)

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  include_directories(${LLVM_INCLUDE_DIRS})
  add_definitions(${LLVM_DEFINITIONS})

  # Find the libraries that correspond to the LLVM components
  # that we wish to use
  llvm_map_components_to_libnames(llvm_libs support core irreader)
  set(EMIT_LLVM_CPP "emit_llvm.cpp")
else (NOT WIN32)
  set(EMIT_LLVM_CPP "")
endif (NOT WIN32)

# if (NOT WIN32)
#   # We currently don't know if python3 if available.
# add_custom_command(
#   OUTPUT Grammar.h Grammar.cpp
#   COMMAND python3 ../simplecompiler.py generate -a grammar -l cpp -o ${CMAKE_CURRENT_BINARY_DIR} ../Grammar
#   DEPENDS ../Grammar
# )

# add_custom_command(
#   OUTPUT AST.h AST.cpp
#   COMMAND python3 ../simplecompiler.py generate -a ast -l cpp -o ${CMAKE_CURRENT_BINARY_DIR} ../asdl
#   DEPENDS ../asdl
# )
# endif()

# All executables added hereby
add_executable(codegen
  ${MAIN_CPP}
  ${EMIT_LLVM_CPP}
  ./CompilerInstance.cpp
  ./ByteCodePrinter.cpp
  ./assemble.cpp
  ./ByteCode.cpp
  ./compile.cpp
  ./Opcode.cpp
  ./AST.cpp
  ./Grammar.cpp
  ./type_check.cpp
  ./SymbolTable.cpp
  ./syntax_check.cpp
  ./cst.cpp
  ./parser.cpp
  ./gramdef.cpp
  ./Node.cpp
  ./tokenize.cpp)

if (NOT WIN32)
  # Link against LLVM libraries
  target_link_libraries(codegen ${llvm_libs})
endif (NOT WIN32)

project(Optimize)
set(EXEC_NAME optimize)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

# Find TCLAP comand line library (Required for *nix build)
if (NOT WIN32)
  find_path(TCLAP_INCLUDE
    NAMES tclap/CmdLine.h
    PATHS "$ENV{HOME}/include"
    DOC "Tclap command line library's headers")

  if (NOT TCLAP_INCLUDE)
    message(FATAL_ERROR
      "Tclap is required for *nix build. Please install that from http://tclap.sourceforge.net/")
  endif (NOT TCLAP_INCLUDE)

  message(STATUS ${TCLAP_INCLUDE})
  include_directories(${TCLAP_INCLUDE})
endif (NOT WIN32)


set(USE_LLVM OFF)
# Find LLVM infrustrament (Optional for *nix build)
if (NOT WIN32)
  find_package(LLVM CONFIG)

  if (NOT LLVM_DIR)
    message(STATUS "LLVM Infrustrament not found, build without it")
  else (NOT LLVM_DIR)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    # Find the libraries that correspond to the LLVM components
    # that we wish to use
    llvm_map_components_to_libnames(llvm_libs support core irreader)
    set(USE_LLVM ON)
  endif (NOT LLVM_DIR)

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

set(SOURCE_FILES
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
  ./AstBuilder.cpp
  ./parser.cpp
  ./gramdef.cpp
  ./Node.cpp
  ./tokenize.cpp)

set(EXTRA_LIBRARIES "")

if (USE_LLVM)
  list(APPEND SOURCE_FILES
    ./CSTGraph.cpp
    ./ASTGraph.cpp
    ./emit_llvm.cpp)
  list(APPEND EXTRA_LIBRARIES ${llvm_libs})
  add_definitions(-DSIMPLE_COMPILER_USE_LLVM)
  include_directories(${LLVM_INCLUDE_DIRS})
  add_definitions(${LLVM_DEFINITIONS})
endif(USE_LLVM)

if (WIN32)
  list(APPEND SOURCE_FILES ./windows_main.cpp)
else (WIN32)
  list(APPEND SOURCE_FILES ./main.cpp)
endif (WIN32)

add_executable(${EXEC_NAME} ${SOURCE_FILES})
target_link_libraries(${EXEC_NAME} ${EXTRA_LIBRARIES})

project(simplecompiler)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
include_directories(include)

# Find TCLAP comand line library (Required for *nix build)
if (NOT MSVC)
  find_path(TCLAP_INCLUDE
    NAMES tclap/CmdLine.h
    PATHS "$ENV{HOME}/include"
    DOC "Tclap command line library's headers")
  if (NOT TCLAP_INCLUDE)
    message(FATAL_ERROR
      "Tclap is required for *nix build. Please install that from http://tclap.sourceforge.net/")
  endif ()
  message(STATUS ${TCLAP_INCLUDE})
  include_directories(${TCLAP_INCLUDE})
  set(CMAKE_CXX_FLAGS "-Wunused-function ${CMAKE_CXX_FLAGS}")
endif ()


# Find LLVM infrustrament (Optional for *nix build)
set(USE_LLVM OFF)
if (NOT MSVC)
  find_package(LLVM CONFIG)
  if (NOT LLVM_DIR)
    message(STATUS "LLVM Infrustrament not found, build without it")
  else ()
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    # Find the libraries that correspond to the LLVM components
    # that we wish to use
    llvm_map_components_to_libnames(llvm_libs support core irreader)
    set(USE_LLVM ON)
  endif ()
endif ()

# if (NOT MSVC)
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

add_subdirectory(lib)

project(simplecc)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 11)
include_directories(include)

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

if (USE_LLVM)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(-DSIMPLE_COMPILER_USE_LLVM=1)
    add_definitions(${LLVM_DEFINITIONS})
endif ()

add_subdirectory(lib)
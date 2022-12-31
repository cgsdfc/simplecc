# MIT License

# Copyright (c) 2018 Cong Feng.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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

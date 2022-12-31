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

# Add all components.
add_subdirectory(Lex)
add_subdirectory(Parse)
add_subdirectory(AST)
add_subdirectory(Analysis)
add_subdirectory(CodeGen)
add_subdirectory(Transform)
add_subdirectory(Target)
add_subdirectory(Driver)

# Add main executable.
add_executable(simplecc Driver/main.cpp)

# Link to all components.
target_link_libraries(simplecc Driver)

if (USE_LLVM)
    add_subdirectory(EmitLLVM)
    add_subdirectory(Visualize)

    target_link_libraries(simplecc ${llvm_libs})
    target_link_libraries(simplecc EmitLLVM Visualize)
endif ()

install(TARGETS simplecc DESTINATION bin)

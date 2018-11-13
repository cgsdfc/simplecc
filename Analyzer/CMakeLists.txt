project(Analyzer)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

# All executables added hereby
add_executable(analyzer
  ./validate.cpp
  ./AST.cpp
  ./main.cpp
  ./cst.cpp
  ./Grammar.cpp
  ./parser.cpp
  ./gramdef.cpp
  ./tokenize.cpp)

project(Parser)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_CXX_FLAGS "-std=c++17")

# All executables added hereby
add_executable(parser
  ./AST.cpp
  ./main.cpp
  ./cst.cpp
  ./Grammar.cpp
  ./parser.cpp
  ./tokenize.cpp)

project(Codegen)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

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

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# All executables added hereby
add_executable(codegen
  ./mips.cpp
  ./code.cpp
  ./compile.cpp
  ./Opcode.cpp
  ./AST.cpp
  ./Grammar.cpp
  ./type_check.cpp
  ./symtable.cpp
  ./syntax_check.cpp
  ./main.cpp
  ./cst.cpp
  ./parser.cpp
  ./gramdef.cpp
  ./tokenize.cpp)

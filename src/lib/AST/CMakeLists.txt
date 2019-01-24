add_library(AST STATIC
        AST.cpp
        ASTPrettyPrinter.cpp
        ASTVerifier.cpp
        Enums.cpp)

target_link_libraries(AST Lex)
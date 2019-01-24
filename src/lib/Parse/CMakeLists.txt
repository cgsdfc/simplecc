add_library(Parse STATIC
        ASTBuilder.cpp
        Grammar.cpp
        Node.cpp
        Parse.cpp
        Parser.cpp
        ParseTreePrinter.cpp)

target_link_libraries(Parse Lex AST)
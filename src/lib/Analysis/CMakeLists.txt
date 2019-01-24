add_library(Analysis STATIC
        AnalysisManager.cpp
        ArrayBoundChecker.cpp
        ImplicitCallTransformer.cpp
        SymbolTable.cpp
        SymbolTableBuilder.cpp
        SyntaxChecker.cpp
        TypeChecker.cpp
        TypeEvaluator.cpp
        Types.cpp)

target_link_libraries(Analysis AST Lex)
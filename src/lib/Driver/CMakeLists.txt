add_library(Driver STATIC
        Driver.cpp
        DriverBase.cpp
        WindowsDriver.cpp)

target_link_libraries(Driver
        Lex
        Parse
        AST
        Analysis
        CodeGen
        Target
        IR
        Transform)
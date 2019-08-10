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

install(TARGETS simplecc DESTINATION bin/)
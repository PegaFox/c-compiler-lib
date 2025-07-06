cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)

project(c-compiler-lib)

add_library(c-compiler-lib STATIC C_compiler.cpp generate_IR.cpp handle_args.cpp lexer.cpp load_file.cpp optimize_IR.cpp optimize_AST.cpp parser.cpp preprocessor.cpp print_IR.cpp print_AST.cpp remove_sub_expressions.cpp save_file.cpp)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE c-compiler-lib)

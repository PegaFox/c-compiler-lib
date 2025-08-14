cmake_minimum_required(VERSION 3.7...3.30)

add_executable(IR_only main.cpp)

target_link_libraries(IR_only PRIVATE c-compiler-lib)

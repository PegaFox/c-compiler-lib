cmake_minimum_required(VERSION 3.7...3.30)

add_executable(x86_backend main.cpp)

target_link_libraries(x86_backend PRIVATE c-compiler-lib)

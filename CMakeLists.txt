cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)

project(c-compiler-lib)

add_library(c-compiler-lib STATIC
  preprocessor.cpp
  lexer.cpp

  parser/array.cpp
  parser/data_type.cpp
  parser/struct.cpp
  parser/if_conditional.cpp
  parser/program.cpp
  parser/ternary_operator.cpp
  parser/AST_iterator.cpp
  parser/do_while_loop.cpp
  parser/label.cpp
  parser/return.cpp
  parser/type_cast.cpp
  parser/binary_operator.cpp
  parser/expression.cpp
  parser/parse_error.cpp
  parser/statement.cpp
  parser/variable_access.cpp
  parser/break.cpp    
  parser/for_loop.cpp
  parser/pointer.cpp
  parser/sub_expression.cpp
  parser/declaration.cpp
  parser/compound_statement.cpp
  parser/function_call.cpp
  parser/post_unary_operator.cpp
  parser/switch_case.cpp       
  parser/while_loop.cpp
  parser/constant.cpp        
  parser/function.cpp 
  parser/pre_unary_operator.cpp 
  parser/switch_conditional.cpp
  parser/continue.cpp      
  parser/goto.cpp            
  parser/primitive_type.cpp    
  parser/switch_default.cpp

  print_AST.cpp

  generate_IR.cpp
  optimize_IR.cpp
  print_IR.cpp

  compiler.cpp

  save_file.cpp
)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE c-compiler-lib)

#ifndef PF_PARSER_STRUCT_HPP
#define PF_PARSER_STRUCT_HPP

#include <vector>

#include "variable_declaration.hpp"

struct Struct: public DataType
{
  std::string identifier;

  std::vector<std::unique_ptr<VariableDeclaration>> members;

  Struct();
};

#endif // PF_PARSER_STRUCT_HPP

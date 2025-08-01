#ifndef PF_PARSER_STRUCT_HPP
#define PF_PARSER_STRUCT_HPP

#include <vector>

#include "declaration.hpp"

struct Struct: public DataType
{
  std::string identifier;

  std::vector<std::pair<std::unique_ptr<Declaration>, uint8_t>> members;

  Struct();
};

#endif // PF_PARSER_STRUCT_HPP

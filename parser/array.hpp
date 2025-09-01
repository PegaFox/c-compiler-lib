#ifndef PF_PARSER_ARRAY_HPP
#define PF_PARSER_ARRAY_HPP

#include <memory>

#include "expression.hpp"
#include "data_type.hpp"

struct Array: public DataType
{
  DataType* dataType = nullptr;
  Expression* size = nullptr;

  Array();

  static Array* parse(std::list<Token>& code);
};

#endif // PF_PARSER_ARRAY_HPP

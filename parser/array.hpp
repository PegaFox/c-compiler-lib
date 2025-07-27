#ifndef PF_PARSER_ARRAY_HPP
#define PF_PARSER_ARRAY_HPP

#include <memory>

#include "expression.hpp"
#include "data_type.hpp"

struct Array: public DataType
{
  std::unique_ptr<DataType> dataType;
  std::unique_ptr<Expression> size;

  Array();
};

#endif // PF_PARSER_ARRAY_HPP

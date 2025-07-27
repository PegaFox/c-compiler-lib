#ifndef PF_PARSER_TYPE_CAST_HPP
#define PF_PARSER_TYPE_CAST_HPP

#include "data_type.hpp"
#include "pre_unary_operator.hpp"

struct TypeCast: public PreUnaryOperator
{
  std::unique_ptr<DataType> dataType;

  TypeCast();
};

#endif // PF_PARSER_TYPE_CAST_HPP

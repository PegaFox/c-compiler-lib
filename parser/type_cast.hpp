#ifndef PF_PARSER_TYPE_CAST_HPP
#define PF_PARSER_TYPE_CAST_HPP

#include "data_type.hpp"
#include "pre_unary_operator.hpp"

struct TypeCast: public PreUnaryOperator
{
  DataType* dataType = nullptr;

  TypeCast();

  static TypeCast* parse(CommonParseData& data);
};

#endif // PF_PARSER_TYPE_CAST_HPP

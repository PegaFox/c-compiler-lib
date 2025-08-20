#ifndef PF_PARSER_CONSTANT_HPP
#define PF_PARSER_CONSTANT_HPP

#include <memory>

#include "primitive_type.hpp"
#include "expression.hpp"

struct Constant: public Expression
{
  PrimitiveType dataType;

  uint8_t value[16] = {0};

  Constant();

  static Constant* parse(CommonParseData& data);
};

#endif // PF_PARSER_CONSTANT_HPP

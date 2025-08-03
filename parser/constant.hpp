#ifndef PF_PARSER_CONSTANT_HPP
#define PF_PARSER_CONSTANT_HPP

#include <memory>

#include "data_type.hpp"
#include "expression.hpp"

struct Constant: public Expression
{
  std::unique_ptr<DataType> dataType;

  uint8_t value[16];

  Constant();

  static Constant* parse(CommonParseData& data);
};

#endif // PF_PARSER_CONSTANT_HPP

#ifndef PF_PARSER_CONSTANT_HPP
#define PF_PARSER_CONSTANT_HPP

#include <memory>

#include "primitive_type.hpp"
#include "expression.hpp"

struct Constant: public Expression
{
  PrimitiveType dataType;
  
  // Raw binary representation of the constant
  uint8_t value[16] = {0};

  Constant();

  static Constant* parse(CommonParseData& data);

  static Constant parseFromString(
    const std::map<std::string, ENUM_TYPE>& enums,
    Compiler::TypeSizes typeSizes,
    const std::string& token);
};

#endif // PF_PARSER_CONSTANT_HPP

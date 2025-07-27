#ifndef PF_PARSER_PRIMITIVE_TYPE_HPP
#define PF_PARSER_PRIMITIVE_TYPE_HPP

#include "data_type.hpp"

struct PrimitiveType: public DataType
{
  enum class Type
  {
    Void,
    SignedChar,
    UnsignedChar,
    SignedShort,
    UnsignedShort,
    SignedInt,
    UnsignedInt,
    SignedLong,
    UnsignedLong,
    SignedLongLong,
    UnsignedLongLong,
    Float,
    Double,
    LongDouble
  } type;

  PrimitiveType();

  static PrimitiveType* parse(std::list<Token>& code);
};

#endif // PF_PARSER_PRIMITIVE_TYPE_HPP

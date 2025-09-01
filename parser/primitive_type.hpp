#ifndef PF_PARSER_PRIMITIVE_TYPE_HPP
#define PF_PARSER_PRIMITIVE_TYPE_HPP

#include "data_type.hpp"

struct PrimitiveType: public DataType
{
  /*enum class Type
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
  } type;*/

  bool isFloating = false;
  bool isSigned = false;
  uint16_t size = 0;
  uint8_t alignment = 0;

  PrimitiveType();

  PrimitiveType(uint16_t size, uint8_t alignment, bool isFloating = false, bool isSigned = false, bool isVolatile = false);

  static PrimitiveType* parse(std::list<Token>& code);
};

#endif // PF_PARSER_PRIMITIVE_TYPE_HPP

#include "primitive_type.hpp"

PrimitiveType::PrimitiveType()
{
  generalType = GeneralType::PrimitiveType;
}

PrimitiveType::PrimitiveType(uint16_t size, uint8_t alignment, bool isFloating, bool isSigned, bool isConst, bool isVolatile): size(size), alignment(alignment), isFloating(isFloating), isSigned(isSigned)
{
  generalType = GeneralType::PrimitiveType;

  this->isConst = isConst;
  this->isVolatile = isVolatile;
}

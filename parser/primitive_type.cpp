#include "primitive_type.hpp"

PrimitiveType::PrimitiveType()
{
  generalType = GeneralType::PrimitiveType;
}

PrimitiveType::PrimitiveType(bool isFloating, bool isSigned, uint8_t size): isFloating(isFloating), isSigned(isSigned), size(size)
{
  generalType = GeneralType::PrimitiveType;
}

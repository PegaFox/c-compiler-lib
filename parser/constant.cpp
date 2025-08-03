#include "constant.hpp"

#include "primitive_type.hpp"

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

Constant* Constant::parse(CommonParseData& data)
{
  Constant* constant = new Constant;
  PrimitiveType* constantType = new PrimitiveType;

  if (data.code.front().data.front() == '\'' && data.code.front().data.back() == '\'')
  { // char constant
    *constantType = PrimitiveType{false, true, data.typeSizes.charSize};
    constant->value.signedChar = data.code.front().data[1];
    data.code.pop_front();
  } else if (data.code.front().data.front() == '\"' && data.code.front().data.back() == '\"')
  { // str constant

  } else if (data.code.front().data.front() >= '0' && data.code.front().data.front() <= '9')
  { // int constant
    if (data.code.front().data.back() == 'L')
    { // long int
      if (data.code.front().data[data.code.front().data.size()-2] == 'U')
      { // unsigned long int

      } else
      { // long int

      }
    } else if (data.code.front().data.back() == 'U')
    { // unsigned int
      int constVal = std::stoi(data.code.front().data);
      data.code.pop_front();

      if (constVal == (constVal & 0xFF))
      {
        *constantType = PrimitiveType{false, false, data.typeSizes.charSize};
        constant->value.unsignedChar = constVal;
      } else if (constVal == (constVal & 0xFFFF))
      {
        *constantType = PrimitiveType{false, false, data.typeSizes.shortSize};
        constant->value.unsignedShort = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        *constantType = PrimitiveType{false, false, data.typeSizes.longSize};
        constant->value.unsignedLong = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        *constantType = PrimitiveType{false, false, data.typeSizes.longLongSize};
        constant->value.unsignedLongLong = constVal;
      }
    } else
    { // signed int
      int constVal = std::stoi(data.code.front().data);
      data.code.pop_front();

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          *constantType = PrimitiveType{false, true, data.typeSizes.charSize};
          constant->value.signedChar = constVal;
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          *constantType = PrimitiveType{false, true, data.typeSizes.shortSize};
          constant->value.signedShort = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          *constantType = PrimitiveType{false, true, data.typeSizes.longSize};
          constant->value.signedLong = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          *constantType = PrimitiveType{false, true, data.typeSizes.longLongSize};
          constant->value.signedLongLong = constVal;
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          *constantType = PrimitiveType{false, false, data.typeSizes.charSize};
          constant->value.unsignedChar = constVal;
        } else if (constVal == (constVal & 0xFFFF))
        {
          *constantType = PrimitiveType{false, false, data.typeSizes.shortSize};
          constant->value.unsignedShort = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          *constantType = PrimitiveType{false, false, data.typeSizes.longSize};
          constant->value.unsignedLong = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          *constantType = PrimitiveType{false, false, data.typeSizes.longLongSize};
          constant->value.unsignedLongLong = constVal;
        }
      }
    }
  }

  constant->dataType = std::unique_ptr<DataType>(constantType);

  return constant;
}

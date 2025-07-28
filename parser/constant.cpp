#include "constant.hpp"

#include "primitive_type.hpp"

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

Constant* Constant::parse(std::list<Token>& code)
{
  Constant* constant = new Constant;
  PrimitiveType* constantType = new PrimitiveType;

  if (code.front().data.front() == '\'' && code.front().data.back() == '\'')
  { // char constant
    constantType->type = PrimitiveType::Type::SignedChar;
    constant->value.signedChar = code.front().data[1];
    code.pop_front();
  } else if (code.front().data.front() == '\"' && code.front().data.back() == '\"')
  { // str constant

  } else if (code.front().data.front() >= '0' && code.front().data.front() <= '9')
  { // int constant
    if (code.front().data.back() == 'L')
    { // long int
      if (code.front().data[code.front().data.size()-2] == 'U')
      { // unsigned long int

      } else
      { // long int

      }
    } else if (code.front().data.back() == 'U')
    { // unsigned int
      int constVal = std::stoi(code.front().data);
      code.pop_front();

      if (constVal == (constVal & 0xFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedChar;
        constant->value.unsignedChar = constVal;
      } else if (constVal == (constVal & 0xFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedShort;
        constant->value.unsignedShort = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedLong;
        constant->value.unsignedLong = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedLongLong;
        constant->value.unsignedLongLong = constVal;
      }
    } else
    { // signed int
      int constVal = std::stoi(code.front().data);
      code.pop_front();

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          constantType->type = PrimitiveType::Type::SignedChar;
          constant->value.signedChar = constVal;
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedShort;
          constant->value.signedShort = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedLong;
          constant->value.signedLong = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedLongLong;
          constant->value.signedLongLong = constVal;
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedChar;
          constant->value.unsignedChar = constVal;
        } else if (constVal == (constVal & 0xFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedShort;
          constant->value.unsignedShort = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedLong;
          constant->value.unsignedLong = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedLongLong;
          constant->value.unsignedLongLong = constVal;
        }
      }
    }
  }

  constant->dataType = std::unique_ptr<DataType>(constantType);

  return constant;
}

#include "constant.hpp"

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

Constant* Constant::parse(CommonParseData& data)
{
  Constant* constant = new Constant;

  if (data.program->enums.contains(data.code.front().data))
  {
    constant->dataType.isFloating = (ENUM_TYPE(0.5f) == 0.5f);
    constant->dataType.isSigned = (ENUM_TYPE(-1) < ENUM_TYPE(0));
    constant->dataType.size = sizeof(ENUM_TYPE);

    long int constVal = data.program->enums.contains(data.code.front().data);
    std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
    data.code.pop_front();
  } else if (data.code.front().data.front() == '\'' && data.code.front().data.back() == '\'')
  { // char constant
    constant->dataType = PrimitiveType{false, true, data.typeSizes.charSize};
    constant->value[0] = data.code.front().data[1];
    data.code.pop_front();
  } else if (data.code.front().data.front() == '\"' && data.code.front().data.back() == '\"')
  { // str constant

  } else if (data.code.front().data.front() >= '0' && data.code.front().data.front() <= '9')
  { // number constant
    if (data.code.front().data.find('.') != std::string::npos)
    { // float constant
      if (data.code.front().data.back() == 'f' || data.code.front().data.back() == 'F')
      { // float
        float constVal = std::stof(data.code.front().data);
        data.code.pop_front();

        constant->dataType = PrimitiveType{true, true, data.typeSizes.floatSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (data.code.front().data.back() == 'l' || data.code.front().data.back() == 'L')
      { // long double
        long double constVal = std::stold(data.code.front().data);
        data.code.pop_front();

        constant->dataType = PrimitiveType{true, true, data.typeSizes.longDoubleSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else
      { // double
        double constVal = std::stod(data.code.front().data);
        data.code.pop_front();

        constant->dataType = PrimitiveType{true, true, data.typeSizes.doubleSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      }
    } else if (data.code.front().data.back() == 'L')
    { // long int
      if (data.code.front().data[data.code.front().data.size()-2] == 'U')
      { // unsigned long int

      } else
      { // long int

      }
    } else if (data.code.front().data.back() == 'U')
    { // unsigned int
      long int constVal = std::stol(data.code.front().data);
      data.code.pop_front();

      if (constVal == (constVal & 0xFF))
      {
        constant->dataType = PrimitiveType{false, false, data.typeSizes.charSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFF))
      {
        constant->dataType = PrimitiveType{false, false, data.typeSizes.shortSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        constant->dataType = PrimitiveType{false, false, data.typeSizes.longSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        constant->dataType = PrimitiveType{false, false, data.typeSizes.longLongSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      }
    } else
    { // signed int
      int constVal = std::stoi(data.code.front().data);
      data.code.pop_front();

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          constant->dataType = PrimitiveType{false, true, data.typeSizes.charSize};
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          constant->dataType = PrimitiveType{false, true, data.typeSizes.shortSize};
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          constant->dataType = PrimitiveType{false, true, data.typeSizes.longSize};
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant->dataType = PrimitiveType{false, true, data.typeSizes.longLongSize};
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          constant->dataType = PrimitiveType{false, false, data.typeSizes.charSize};
        } else if (constVal == (constVal & 0xFFFF))
        {
          constant->dataType = PrimitiveType{false, false, data.typeSizes.shortSize};
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          constant->dataType = PrimitiveType{false, false, data.typeSizes.longSize};
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant->dataType = PrimitiveType{false, false, data.typeSizes.longLongSize};
        }
      }

      std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
    }
  }

  return constant;
}

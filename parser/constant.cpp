#include "constant.hpp"

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

Constant* Constant::parse(CommonParseData& data)
{
  Constant* constant;
  constant = data.program->arenaAlloc(constant);

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
    constant->dataType = PrimitiveType{data.typeSizes.charSize, data.typeSizes.charSize, false, true};
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

        constant->dataType = PrimitiveType{data.typeSizes.floatSize, data.typeSizes.floatSize, true, true};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (data.code.front().data.back() == 'l' || data.code.front().data.back() == 'L')
      { // long double
        long double constVal = std::stold(data.code.front().data);
        data.code.pop_front();

        constant->dataType = PrimitiveType{data.typeSizes.longDoubleSize, data.typeSizes.longDoubleSize, true, true};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else
      { // double
        double constVal = std::stod(data.code.front().data);
        data.code.pop_front();

        constant->dataType = PrimitiveType{data.typeSizes.doubleSize, data.typeSizes.doubleSize, true, true};
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
        constant->dataType = PrimitiveType{data.typeSizes.charSize, data.typeSizes.charSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFF))
      {
        constant->dataType = PrimitiveType{data.typeSizes.shortSize, data.typeSizes.shortSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        constant->dataType = PrimitiveType{data.typeSizes.longSize, data.typeSizes.longSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        constant->dataType = PrimitiveType{data.typeSizes.longLongSize, data.typeSizes.longLongSize};
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
          constant->dataType = PrimitiveType{data.typeSizes.charSize, data.typeSizes.charSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.shortSize, data.typeSizes.shortSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.longSize, data.typeSizes.longSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.longLongSize, data.typeSizes.longLongSize, false, true};
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.charSize, data.typeSizes.charSize};
        } else if (constVal == (constVal & 0xFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.shortSize, data.typeSizes.shortSize};
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.longSize, data.typeSizes.longSize};
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant->dataType = PrimitiveType{data.typeSizes.longLongSize, data.typeSizes.longLongSize};
        }
      }

      std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant->dataType.size, constant->value);
    }
  }

  return constant;
}

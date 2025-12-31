#include "constant.hpp"

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

Constant* Constant::parse(CommonParseData& data)
{
  Constant* constant;
  constant = data.program->arenaAlloc(constant);

  *constant = parseFromString(
    data.program->enums,
    data.typeSizes,
    data.code.front().data);

  data.code.pop_front();

  return constant;
}

Constant Constant::parseFromString(
  const std::map<std::string, ENUM_TYPE>& enums,
  Compiler::TypeSizes typeSizes,
  const std::string& token)
{
  Constant constant;

  if (enums.contains(token))
  {
    constant.dataType.isFloating = (ENUM_TYPE(0.5f) == 0.5f);
    constant.dataType.isSigned = (ENUM_TYPE(-1) < ENUM_TYPE(0));
    constant.dataType.size = sizeof(ENUM_TYPE);

    long int constVal = enums.contains(token);
    std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
  } else if (token.front() == '\'' && token.back() == '\'')
  { // char constant
    constant.dataType = PrimitiveType{typeSizes.charSize, typeSizes.charSize, false, true};
    constant.value[0] = token[1];
  } else if (token.front() == '\"' && token.back() == '\"')
  { // str constant

  } else if (token.front() >= '0' && token.front() <= '9')
  { // number constant
    if (token.find('.') != std::string::npos)
    { // float constant
      if (token.back() == 'f' || token.back() == 'F')
      { // float
        float constVal = std::stof(token);

        constant.dataType = PrimitiveType{typeSizes.floatSize, typeSizes.floatSize, true, true};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      } else if (token.back() == 'l' || token.back() == 'L')
      { // long double
        long double constVal = std::stold(token);

        constant.dataType = PrimitiveType{typeSizes.longDoubleSize, typeSizes.longDoubleSize, true, true};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      } else
      { // double
        double constVal = std::stod(token);

        constant.dataType = PrimitiveType{typeSizes.doubleSize, typeSizes.doubleSize, true, true};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      }
    } else if (token.back() == 'L')
    { // long int
      if (token[token.size()-2] == 'U')
      { // unsigned long int

      } else
      { // long int

      }
    } else if (token.back() == 'U')
    { // unsigned int
      long int constVal = std::stol(token);

      if (constVal == (constVal & 0xFF))
      {
        constant.dataType = PrimitiveType{typeSizes.charSize, typeSizes.charSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      } else if (constVal == (constVal & 0xFFFF))
      {
        constant.dataType = PrimitiveType{typeSizes.shortSize, typeSizes.shortSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        constant.dataType = PrimitiveType{typeSizes.longSize, typeSizes.longSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        constant.dataType = PrimitiveType{typeSizes.longLongSize, typeSizes.longLongSize};
        std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
      }
    } else
    { // signed int
      int constVal = std::stoi(token);

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          constant.dataType = PrimitiveType{typeSizes.charSize, typeSizes.charSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.shortSize, typeSizes.shortSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.longSize, typeSizes.longSize, false, true};
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.longLongSize, typeSizes.longLongSize, false, true};
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          constant.dataType = PrimitiveType{typeSizes.charSize, typeSizes.charSize};
        } else if (constVal == (constVal & 0xFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.shortSize, typeSizes.shortSize};
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.longSize, typeSizes.longSize};
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constant.dataType = PrimitiveType{typeSizes.longLongSize, typeSizes.longLongSize};
        }
      }

      std::copy((uint8_t*)(&constVal), (uint8_t*)(&constVal)+constant.dataType.size, constant.value);
    }
  }

  return constant;
}

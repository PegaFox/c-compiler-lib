#include "constant.hpp"

#include <array>

#include "parse_error.hpp"

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

  constant.dataType = parseDataType(enums, typeSizes, token);

  std::array<uint8_t, 16> value = parseValue(enums, constant.dataType, token);
  std::copy(value.begin(), value.end(), constant.value);

  return constant;
}

PrimitiveType Constant::parseDataType(
  const std::map<std::string, ENUM_TYPE>& enums,
  Compiler::TypeSizes typeSizes,
  const std::string& token)
{
  if (enums.contains(token))
  {
    return PrimitiveType{
      sizeof(ENUM_TYPE),
      sizeof(ENUM_TYPE),
      ENUM_TYPE(0.5f) == 0.5f,
      ENUM_TYPE(-1) < ENUM_TYPE(0)
    };
  } else if (token.front() == '\'' && token.back() == '\'')
  { // char constant
    return PrimitiveType{
      typeSizes.charSize,
      typeSizes.charSize,
      false,
      true
    };
  } else if (token.front() == '\"' && token.back() == '\"')
  { // str constant

  } else if (token.front() >= '0' && token.front() <= '9')
  { // number constant
    if (token.find('.') != std::string::npos)
    { // float constant
      if (token.back() == 'f' || token.back() == 'F')
      { // float
        return PrimitiveType{
          typeSizes.floatSize,
          typeSizes.floatSize,
          true,
          true
        };
      } else if (token.back() == 'l' || token.back() == 'L')
      { // long double
        return PrimitiveType{
          typeSizes.longDoubleSize,
          typeSizes.longDoubleSize,
          true,
          true
        };
      } else
      { // double
        return PrimitiveType{
          typeSizes.doubleSize,
          typeSizes.doubleSize,
          true,
          true
        };
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
        return PrimitiveType{
          typeSizes.charSize,
          typeSizes.charSize,
          false,
          false
        };
      } else if (constVal == (constVal & 0xFFFF))
      {
        return PrimitiveType{
          typeSizes.shortSize,
          typeSizes.shortSize,
          false,
          false
        };
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        return PrimitiveType{
          typeSizes.longSize,
          typeSizes.longSize,
          false,
          false
        };
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        return PrimitiveType{
          typeSizes.longLongSize,
          typeSizes.longLongSize,
          false,
          false
        };
      }
    } else
    { // signed int
      int constVal = std::stoi(token);

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          return PrimitiveType{
            typeSizes.charSize,
            typeSizes.charSize,
            false,
            true
          };
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          return PrimitiveType{
            typeSizes.shortSize,
            typeSizes.shortSize,
            false,
            true
          };
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          return PrimitiveType{
            typeSizes.longSize,
            typeSizes.longSize,
            false,
            true
          };
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          return PrimitiveType{
            typeSizes.longLongSize,
            typeSizes.longLongSize,
            false,
            true
          };
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          return PrimitiveType{
            typeSizes.charSize,
            typeSizes.charSize,
            false,
            false
          };
        } else if (constVal == (constVal & 0xFFFF))
        {
          return PrimitiveType{
            typeSizes.shortSize,
            typeSizes.shortSize,
            false,
            false
          };
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          return PrimitiveType{
            typeSizes.longSize,
            typeSizes.longSize,
            false,
            false
          };
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          return PrimitiveType{
            typeSizes.longLongSize,
            typeSizes.longLongSize,
            false,
            false
          };
        }
      }
    }
  }

  std::cout << "Parse error: Invalid constant\n";
  throw ParseError();
  //return dataType;
}

std::array<uint8_t, 16> Constant::parseValue(
  const std::map<std::string, ENUM_TYPE>& enums,
  PrimitiveType dataType,
  const std::string& token)
{
  std::array<uint8_t, 16> value;

  if (enums.contains(token))
  {
    ENUM_TYPE constVal = enums.at(token);
    uint8_t* begin = (uint8_t*)&constVal;
    
    std::copy(begin, begin+dataType.size, value.begin());
  } else if (token.front() == '\'' && token.back() == '\'')
  { // char constant
    value[0] = token[1];
  } else if (token.front() == '\"' && token.back() == '\"')
  { // str constant

  } else if (dataType.isFloating)
  { // float constant
    switch (dataType.size)
    {
      case 4:
      {
        float constVal = std::stof(token);
        uint8_t* begin = (uint8_t*)&constVal;

        std::copy(begin, begin+dataType.size, value.begin());
        break;
      } case 8:
      {
        double constVal = std::stod(token);
        uint8_t* begin = (uint8_t*)&constVal;

        std::copy(begin, begin+dataType.size, value.begin());
        break;
      } case 10:
      case 16:
      {
        if (sizeof(dataType) == dataType.size)
        {
          long double constVal = std::stold(token);
          uint8_t* begin = (uint8_t*)&constVal;

          std::copy(begin, begin+dataType.size, value.begin());
        }
        break;
      }
    }
  } else if (dataType.isSigned)
  { // signed int
    int64_t constVal = std::stoll(token);
    uint8_t* begin = (uint8_t*)&constVal;

    std::copy(begin, begin+dataType.size, value.begin());
  } else
  { // unsigned int
    uint64_t constVal = std::stoull(token);
    uint8_t* begin = (uint8_t*)&constVal;

    std::copy(begin, begin+dataType.size, value.begin());
  }

  return value;
}

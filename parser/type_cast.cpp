#include "type_cast.hpp"

TypeCast::TypeCast()
{
  preUnaryType = PreUnaryType::TypeCast;
}

TypeCast* TypeCast::parse(CommonParseData& data)
{
  TypeCast* typeCast = new TypeCast;

  if (data.code.front().data == "(")
  {
    data.code.pop_front();
  }

  typeCast->dataType = std::unique_ptr<DataType>(DataType::parse(data));

  if (data.code.front().data == ")")
  {
    data.code.pop_front();
  }

  return typeCast;
}

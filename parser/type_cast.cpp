#include "type_cast.hpp"

TypeCast::TypeCast()
{
  preUnaryType = PreUnaryType::TypeCast;
}

TypeCast* TypeCast::parse(CommonParseData& data)
{
  TypeCast* typeCast;
  typeCast = data.program->arenaAlloc(typeCast);

  if (data.code.front().data == "(")
  {
    data.code.pop_front();
  }

  typeCast->dataType = DataType::parse(data);

  if (data.code.front().data == ")")
  {
    data.code.pop_front();
  }

  return typeCast;
}

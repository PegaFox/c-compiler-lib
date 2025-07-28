#include "type_cast.hpp"

TypeCast::TypeCast()
{
  preUnaryType = PreUnaryType::TypeCast;
}

TypeCast* TypeCast::parse(std::list<Token>& code)
{
  TypeCast* typeCast = new TypeCast;

  if (code.front().data == "(")
  {
    code.pop_front();
  }

  typeCast->dataType = std::unique_ptr<DataType>(DataType::parse(code));

  if (code.front().data == ")")
  {
    code.pop_front();
  }

  return typeCast;
}

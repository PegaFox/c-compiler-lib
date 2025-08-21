#include "string_literal.hpp"

StringLiteral::StringLiteral()
{
  expressionType = ExpressionType::StringLiteral;
}

StringLiteral* StringLiteral::parse(CommonParseData& data)
{
  StringLiteral* stringLiteral = new StringLiteral;

  data.code.front().data.pop_back();
  stringLiteral->value = data.code.front().data.substr(1);

  data.code.pop_front();

  return stringLiteral;
}

#include "if_conditional.hpp"

#include "parse_error.hpp"

IfConditional::IfConditional()
{
  statementType = StatementType::IfConditional;
}

IfConditional* IfConditional::parse(CommonParseData& data)
{
  IfConditional* ifConditional;
  ifConditional = data.program->arenaAlloc(ifConditional);

  ParseError::expect(data.code.front().data, "if");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  ifConditional->condition = Expression::parse(data, false);

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  ifConditional->body = Statement::parse(data);

  if (data.code.front().data == "else")
  {
    data.code.pop_front();
    ifConditional->elseStatement = Statement::parse(data);
  }

  return ifConditional;
}

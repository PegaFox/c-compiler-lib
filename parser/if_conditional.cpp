#include "if_conditional.hpp"

#include "parse_error.hpp"

IfConditional::IfConditional()
{
  statementType = StatementType::IfConditional;
}

IfConditional* IfConditional::parse(CommonParseData& data)
{
  IfConditional* ifConditional = new IfConditional;

  ParseError::expect(data.code.front().data, "if");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  ifConditional->condition = std::unique_ptr<Expression>(Expression::parse(data, false));

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  ifConditional->body = std::unique_ptr<Statement>(Statement::parse(data));

  if (data.code.front().data == "else")
  {
    data.code.pop_front();
    ifConditional->elseStatement = std::unique_ptr<Statement>(Statement::parse(data));
  }

  return ifConditional;
}

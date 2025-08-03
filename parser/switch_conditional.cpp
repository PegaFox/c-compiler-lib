#include "switch_conditional.hpp"

#include "parse_error.hpp"

SwitchConditional::SwitchConditional()
{
  statementType = StatementType::SwitchConditional;
}

SwitchConditional* SwitchConditional::parse(CommonParseData& data)
{
  SwitchConditional* switchConditional = new SwitchConditional;

  ParseError::expect(data.code.front().data, "switch");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  switchConditional->value = std::unique_ptr<Expression>(Expression::parse(data, false));

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  switchConditional->body = std::unique_ptr<Statement>(Statement::parse(data));

  return switchConditional;
}

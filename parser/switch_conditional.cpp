#include "switch_conditional.hpp"

#include "parse_error.hpp"

SwitchConditional::SwitchConditional()
{
  statementType = StatementType::SwitchConditional;
}

SwitchConditional* SwitchConditional::parse(std::list<Token>& code)
{
  SwitchConditional* switchConditional = new SwitchConditional;

  ParseError::expect(code.front().data, "switch");
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  switchConditional->value = std::unique_ptr<Expression>(Expression::parse(code, false));

  ParseError::expect(code.front().data, ")");
  code.pop_front();

  switchConditional->body = std::unique_ptr<Statement>(Statement::parse(code));

  return switchConditional;
}

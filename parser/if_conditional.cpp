#include "if_conditional.hpp"

#include "parse_error.hpp"

IfConditional::IfConditional()
{
  statementType = StatementType::IfConditional;
}

IfConditional* IfConditional::parse(std::list<Token>& code, Program& program)
{
  IfConditional* ifConditional = new IfConditional;

  ParseError::expect(code.front().data, "if");
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  ifConditional->condition = std::unique_ptr<Expression>(Expression::parse(code, program, false));

  ParseError::expect(code.front().data, ")");
  code.pop_front();

  ifConditional->body = std::unique_ptr<Statement>(Statement::parse(code, program));

  if (code.front().data == "else")
  {
    code.pop_front();
    ifConditional->elseStatement = std::unique_ptr<Statement>(Statement::parse(code, program));
  }

  return ifConditional;
}

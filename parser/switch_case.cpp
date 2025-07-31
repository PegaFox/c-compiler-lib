#include "switch_case.hpp"

#include "parse_error.hpp"

SwitchCase::SwitchCase()
{
  statementType = StatementType::SwitchCase;
}

SwitchCase* SwitchCase::parse(std::list<Token>& code, Program& program)
{
  SwitchCase* switchCase = new SwitchCase;

  ParseError::expect(code.front().data, "case");
  code.pop_front();

  switchCase->requirement = std::unique_ptr<Expression>(Expression::parse(code, program, false));

  ParseError::expect(code.front().data, ":");
  code.pop_front();

  return switchCase;
}

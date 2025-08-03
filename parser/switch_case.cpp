#include "switch_case.hpp"

#include "parse_error.hpp"

SwitchCase::SwitchCase()
{
  statementType = StatementType::SwitchCase;
}

SwitchCase* SwitchCase::parse(CommonParseData& data)
{
  SwitchCase* switchCase = new SwitchCase;

  ParseError::expect(data.code.front().data, "case");
  data.code.pop_front();

  switchCase->requirement = std::unique_ptr<Expression>(Expression::parse(data, false));

  ParseError::expect(data.code.front().data, ":");
  data.code.pop_front();

  return switchCase;
}

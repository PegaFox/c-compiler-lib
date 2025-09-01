#include "switch_conditional.hpp"

#include "parse_error.hpp"

SwitchConditional::SwitchConditional()
{
  statementType = StatementType::SwitchConditional;
}

SwitchConditional* SwitchConditional::parse(CommonParseData& data)
{
  SwitchConditional* switchConditional;
  switchConditional = data.program->arenaAlloc(switchConditional);

  ParseError::expect(data.code.front().data, "switch");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  switchConditional->value = Expression::parse(data, false);

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  switchConditional->body = Statement::parse(data);

  return switchConditional;
}

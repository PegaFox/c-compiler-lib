#include "switch_default.hpp"

#include "parse_error.hpp"

SwitchDefault::SwitchDefault()
{
  statementType = StatementType::SwitchDefault;
}

SwitchDefault* SwitchDefault::parse(CommonParseData& data)
{
  SwitchDefault* switchDefault;
  switchDefault = data.program->arenaAlloc(switchDefault);

  ParseError::expect(data.code.front().data, "default");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ":");
  data.code.pop_front();

  return switchDefault;
}

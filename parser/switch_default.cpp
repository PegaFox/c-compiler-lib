#include "switch_default.hpp"

#include "parse_error.hpp"

SwitchDefault::SwitchDefault()
{
  statementType = StatementType::SwitchDefault;
}

SwitchDefault* SwitchDefault::parse(std::list<Token>& code)
{
  SwitchDefault* switchDefault = new SwitchDefault;

  ParseError::expect(code.front().data, "default");
  code.pop_front();

  ParseError::expect(code.front().data, ":");
  code.pop_front();

  return switchDefault;
}

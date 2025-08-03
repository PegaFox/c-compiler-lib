#include "break.hpp"

#include "parse_error.hpp"

Break::Break()
{
  statementType = StatementType::Break;
}

Break* Break::parse(CommonParseData& data)
{
  Break* breakStatement = new Break;

  ParseError::expect(data.code.front().data, "break");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  return breakStatement;
}

#include "return.hpp"

#include "parse_error.hpp"

Return::Return()
{
  statementType = StatementType::Return;
}

Return* Return::parse(CommonParseData& data)
{
  Return* returnVal;
  returnVal = data.program->arenaAlloc(returnVal);

  ParseError::expect(data.code.front().data, "return");
  data.code.pop_front();

  returnVal->data = Expression::parse(data);

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  return returnVal;
}

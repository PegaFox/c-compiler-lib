#include "continue.hpp"

#include "parse_error.hpp"

Continue::Continue()
{
  statementType = StatementType::Continue;
}

Continue* Continue::parse(CommonParseData& data)
{
  Continue* continueStatement = new Continue;

  ParseError::expect(data.code.front().data, "continue");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  return continueStatement;
}

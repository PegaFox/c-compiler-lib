#include "goto.hpp"

#include "parse_error.hpp"

Goto::Goto()
{
  statementType = StatementType::Goto;
}

Goto* Goto::parse(CommonParseData& data)
{
  Goto* gotoStatement;
  gotoStatement = data.program->arenaAlloc(gotoStatement);

  ParseError::expect(data.code.front().data, "goto");
  data.code.pop_front();

  ParseError::expect(data.code.front(), Token::Identifier);
  gotoStatement->label = data.code.front().data;
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  return gotoStatement;
}

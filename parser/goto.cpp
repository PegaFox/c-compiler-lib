#include "goto.hpp"

#include "parse_error.hpp"

Goto::Goto()
{
  statementType = StatementType::Goto;
}

Goto* Goto::parse(std::list<Token>& code)
{
  Goto* gotoStatement = new Goto;

  ParseError::expect(code.front().data, "goto");
  code.pop_front();

  ParseError::expect(code.front(), Token::Identifier);
  gotoStatement->label = code.front().data;
  code.pop_front();

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  return gotoStatement;
}

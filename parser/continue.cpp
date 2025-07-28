#include "continue.hpp"

#include "parse_error.hpp"

Continue::Continue()
{
  statementType = StatementType::Continue;
}

Continue* Continue::parse(std::list<Token>& code)
{
  Continue* continueStatement = new Continue;

  ParseError::expect(code.front().data, "continue");
  code.pop_front();

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  return continueStatement;
}

#include "break.hpp"

#include "parse_error.hpp"

Break::Break()
{
  statementType = StatementType::Break;
}

Break* Break::parse(std::list<Token>& code)
{
  Break* breakStatement = new Break;

  ParseError::expect(code.front().data, "break");
  code.pop_front();

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  return breakStatement;
}

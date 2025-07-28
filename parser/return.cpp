#include "return.hpp"

#include "parse_error.hpp"

Return::Return()
{
  statementType = StatementType::Return;
}

Return* Return::parse(std::list<Token>& code)
{
  Return* returnVal = new Return;

  ParseError::expect(code.front().data, "return");
  code.pop_front();

  returnVal->data = std::unique_ptr<Expression>(Expression::parse(code));

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  return returnVal;
}

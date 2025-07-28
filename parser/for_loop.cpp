#include "for_loop.hpp"

#include "parse_error.hpp"
#include "variable_declaration.hpp"

ForLoop::ForLoop()
{
  statementType = StatementType::ForLoop;
}

ForLoop* ForLoop::parse(std::list<Token>& code)
{
  ForLoop* forLoop = new ForLoop;

  ParseError::expect(code.front().data, "for");
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  if (code.front().data != ";")
  {
    if (code.front().type == Token::Keyword)
    {
      forLoop->initialization = std::unique_ptr<VariableDeclaration>(VariableDeclaration::parse(code));
    } else
    {
      forLoop->initialization = std::unique_ptr<Expression>(Expression::parse(code));
    }
  }
  code.pop_front();
  

  forLoop->condition = std::unique_ptr<Expression>(Expression::parse(code));

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  forLoop->update = std::unique_ptr<Expression>(Expression::parse(code));

  ParseError::expect(code.front().data, ")");
  code.pop_front();

  forLoop->body = std::unique_ptr<Statement>(Statement::parse(code));

  return forLoop;
}

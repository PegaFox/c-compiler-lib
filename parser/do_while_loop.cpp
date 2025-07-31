#include "do_while_loop.hpp"

#include "parse_error.hpp"
#include "expression.hpp"

DoWhileLoop::DoWhileLoop()
{
  statementType = StatementType::DoWhileLoop;
}

DoWhileLoop* DoWhileLoop::parse(std::list<Token>& code, Program& program)
{
  DoWhileLoop* doWhileLoop = new DoWhileLoop;

  ParseError::expect(code.front().data, "do");
  code.pop_front();

  doWhileLoop->body = std::unique_ptr<Statement>(Statement::parse(code, program));

  ParseError::expect(code.front().data, "while");
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  doWhileLoop->condition = std::unique_ptr<Expression>(Expression::parse(code, program, false));

  ParseError::expect(code.front().data, ")");
  code.pop_front();

  ParseError::expect(code.front().data, ";");
  code.pop_front();

  return doWhileLoop;
}

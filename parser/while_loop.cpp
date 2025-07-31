#include "while_loop.hpp"

#include "parse_error.hpp"

WhileLoop::WhileLoop()
{
  statementType = StatementType::WhileLoop;
}

WhileLoop* WhileLoop::parse(std::list<Token>& code, Program& program)
{
  WhileLoop* whileLoop = new WhileLoop;

  ParseError::expect(code.front().data, "while");
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  whileLoop->condition = std::unique_ptr<Expression>(Expression::parse(code, program, false));

  ParseError::expect(code.front().data, ")");
  code.pop_front();

  whileLoop->body = std::unique_ptr<Statement>(Statement::parse(code, program));

  return whileLoop;
}

#include "while_loop.hpp"

#include "parse_error.hpp"

WhileLoop::WhileLoop()
{
  statementType = StatementType::WhileLoop;
}

WhileLoop* WhileLoop::parse(CommonParseData& data)
{
  WhileLoop* whileLoop;
  whileLoop = data.program->arenaAlloc(whileLoop);

  ParseError::expect(data.code.front().data, "while");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  whileLoop->condition = Expression::parse(data, false);

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  whileLoop->body = Statement::parse(data);

  return whileLoop;
}

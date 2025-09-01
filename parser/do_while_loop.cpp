#include "do_while_loop.hpp"

#include "parse_error.hpp"
#include "expression.hpp"

DoWhileLoop::DoWhileLoop()
{
  statementType = StatementType::DoWhileLoop;
}

DoWhileLoop* DoWhileLoop::parse(CommonParseData& data)
{
  DoWhileLoop* doWhileLoop;
  doWhileLoop = data.program->arenaAlloc(doWhileLoop);

  ParseError::expect(data.code.front().data, "do");
  data.code.pop_front();

  doWhileLoop->body = Statement::parse(data);

  ParseError::expect(data.code.front().data, "while");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  doWhileLoop->condition = Expression::parse(data, false);

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  return doWhileLoop;
}

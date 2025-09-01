#include "for_loop.hpp"

#include "parse_error.hpp"
#include "declaration.hpp"

ForLoop::ForLoop()
{
  statementType = StatementType::ForLoop;
}

ForLoop* ForLoop::parse(CommonParseData& data)
{
  ForLoop* forLoop;
  forLoop = data.program->arenaAlloc(forLoop);

  ParseError::expect(data.code.front().data, "for");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  if (data.code.front().data != ";")
  {
    if (data.code.front().type == Token::Keyword || data.program->typedefs.contains(data.code.front().data))
    {
      forLoop->initialization = Declaration::parse(data);
    } else
    {
      forLoop->initialization = Expression::parse(data);
    }
  }
  data.code.pop_front();
  

  forLoop->condition = Expression::parse(data);

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  forLoop->update = Expression::parse(data);

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  forLoop->body = Statement::parse(data);

  return forLoop;
}

#include "for_loop.hpp"

#include "parse_error.hpp"
#include "declaration.hpp"

ForLoop::ForLoop()
{
  statementType = StatementType::ForLoop;
}

ForLoop* ForLoop::parse(CommonParseData& data)
{
  ForLoop* forLoop = new ForLoop;

  ParseError::expect(data.code.front().data, "for");
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();

  if (data.code.front().data != ";")
  {
    if (data.code.front().type == Token::Keyword)
    {
      forLoop->initialization = std::unique_ptr<Declaration>(Declaration::parse(data));
    } else
    {
      forLoop->initialization = std::unique_ptr<Expression>(Expression::parse(data));
    }
  }
  data.code.pop_front();
  

  forLoop->condition = std::unique_ptr<Expression>(Expression::parse(data));

  ParseError::expect(data.code.front().data, ";");
  data.code.pop_front();

  forLoop->update = std::unique_ptr<Expression>(Expression::parse(data));

  ParseError::expect(data.code.front().data, ")");
  data.code.pop_front();

  forLoop->body = std::unique_ptr<Statement>(Statement::parse(data));

  return forLoop;
}

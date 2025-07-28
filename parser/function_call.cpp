#include "function_call.hpp"

#include "parse_error.hpp"

FunctionCall::FunctionCall()
{
  expressionType = ExpressionType::FunctionCall;
}

FunctionCall* FunctionCall::parse(std::list<Token>& code)
{
  FunctionCall* functionCall = new FunctionCall;

  ParseError::expect(code.front(), Token::Identifier);
  functionCall->identifier = code.front().data;
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();
  
  while (code.front().data != ")")
  {
    if (code.front().data == ",")
    {
      code.pop_front();
    }

    functionCall->arguments.emplace_back(std::unique_ptr<Expression>(Expression::parse(code)));
  }
  
  code.pop_front();

  return functionCall;
}

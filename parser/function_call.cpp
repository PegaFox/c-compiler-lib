#include "function_call.hpp"

#include "parse_error.hpp"

FunctionCall::FunctionCall()
{
  expressionType = ExpressionType::FunctionCall;
}

FunctionCall* FunctionCall::parse(CommonParseData& data)
{
  FunctionCall* functionCall = new FunctionCall;

  ParseError::expect(data.code.front(), Token::Identifier);
  functionCall->identifier = data.code.front().data;
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "(");
  data.code.pop_front();
  
  while (data.code.front().data != ")")
  {
    if (data.code.front().data == ",")
    {
      data.code.pop_front();
    }

    functionCall->arguments.emplace_back(std::unique_ptr<Expression>(Expression::parse(data)));
  }
  
  data.code.pop_front();

  return functionCall;
}

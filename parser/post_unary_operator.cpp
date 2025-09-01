#include "post_unary_operator.hpp"

PostUnaryOperator::PostUnaryOperator()
{
  expressionType = ExpressionType::PostUnaryOperator;
}

PostUnaryOperator* PostUnaryOperator::parse(CommonParseData& data, Expression* operand)
{
  PostUnaryOperator* postUnary;
  postUnary = data.program->arenaAlloc(postUnary);

  if (data.code.front().data == "++")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Increment;
  } else if (data.code.front().data == "--")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Decrement;
  }

  data.code.pop_front();

  postUnary->operand = operand;

  return postUnary;
}

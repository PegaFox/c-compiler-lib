#include "post_unary_operator.hpp"

PostUnaryOperator::PostUnaryOperator()
{
  expressionType = ExpressionType::PostUnaryOperator;
}

PostUnaryOperator* PostUnaryOperator::parse(CommonParseData& data, Expression* operand)
{
  PostUnaryOperator* postUnary = new PostUnaryOperator;

  if (data.code.front().data == "++")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Increment;
  } else if (data.code.front().data == "--")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Decrement;
  }

  data.code.pop_front();

  postUnary->operand = std::unique_ptr<Expression>(operand);

  return postUnary;
}

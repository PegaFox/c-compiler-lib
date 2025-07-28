#include "post_unary_operator.hpp"

PostUnaryOperator::PostUnaryOperator()
{
  expressionType = ExpressionType::PostUnaryOperator;
}

PostUnaryOperator* PostUnaryOperator::parse(std::list<Token>& code, Expression* operand)
{
  PostUnaryOperator* postUnary = new PostUnaryOperator;

  if (code.front().data == "++")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Increment;
  } else if (code.front().data == "--")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Decrement;
  }

  code.pop_front();

  postUnary->operand = std::unique_ptr<Expression>(operand);

  return postUnary;
}

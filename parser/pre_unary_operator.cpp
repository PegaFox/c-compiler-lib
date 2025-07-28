#include "pre_unary_operator.hpp"

#include "type_cast.hpp"
#include "binary_operator.hpp"

PreUnaryOperator::PreUnaryOperator()
{
  expressionType = ExpressionType::PreUnaryOperator;
}

Expression* PreUnaryOperator::parse(std::list<Token>& code)
{
  Expression* preUnary = new PreUnaryOperator;

  if (code.front().type == Token::Keyword || code.front().data == "(")
  {
    preUnary = TypeCast::parse(code);
  } else if (code.front().data == "&")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Address;
  } else if (code.front().data == "*")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Dereference;
  } else if (code.front().data == "-")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::MathematicNegate;
  } else if (code.front().data == "~")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::BitwiseNOT;
  } else if (code.front().data == "!")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::LogicalNegate;
  } else if (code.front().data == "++")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Increment;
  } else if (code.front().data == "--")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Decrement;
  }

  code.pop_front();

  ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(Expression::parse(code, false));

  if (((PreUnaryOperator*)preUnary)->operand->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* bottomOperand = (BinaryOperator*)((PreUnaryOperator*)preUnary)->operand.get();
    while (
      bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator)
    {
      bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
    }
    Expression* operand = ((PreUnaryOperator*)preUnary)->operand.release();
    ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
    bottomOperand->leftOperand = std::unique_ptr<Expression>(preUnary);
    preUnary = operand;
  }

  return preUnary;
}

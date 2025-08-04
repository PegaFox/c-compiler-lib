#include "pre_unary_operator.hpp"

#include "type_cast.hpp"
#include "binary_operator.hpp"

PreUnaryOperator::PreUnaryOperator()
{
  expressionType = ExpressionType::PreUnaryOperator;
}

Expression* PreUnaryOperator::parse(CommonParseData& data)
{
  Expression* preUnary;

  if (data.code.front().type == Token::Keyword || data.code.front().data == "(")
  {
    preUnary = TypeCast::parse(data);
  } else
  {
    preUnary = new PreUnaryOperator;
  }

  if (data.code.front().data == "&")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Address;
  } else if (data.code.front().data == "*")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Dereference;
  } else if (data.code.front().data == "-")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::MathematicNegate;
  } else if (data.code.front().data == "~")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::BitwiseNOT;
  } else if (data.code.front().data == "!")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::LogicalNegate;
  } else if (data.code.front().data == "++")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Increment;
  } else if (data.code.front().data == "--")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Decrement;
  }

  if (((PreUnaryOperator*)preUnary)->preUnaryType != PreUnaryType::TypeCast)
  {
    data.code.pop_front();
  }

  ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(Expression::parse(data, false));

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

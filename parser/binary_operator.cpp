#include "binary_operator.hpp"

#include "parse_error.hpp"
#include "ternary_operator.hpp"

BinaryOperator::BinaryOperator()
{
  expressionType = ExpressionType::BinaryOperator;
}

Expression* BinaryOperator::parse(CommonParseData& data, Expression* leftOperand)
{
  Expression* binary = new BinaryOperator;

  ((BinaryOperator*)binary)->leftOperand = std::unique_ptr<Expression>(leftOperand);

  if (data.code.front().data == "+")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Add;
  } else if (data.code.front().data == "-")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Subtract;
  } else if (data.code.front().data == "*")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Multiply;
  } else if (data.code.front().data == "/")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Divide;
  } else if (data.code.front().data == "%")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Modulo;
  } else if (data.code.front().data == "<<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LeftShift;
  } else if (data.code.front().data == ">>")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::RightShift;
  } else if (data.code.front().data == "|")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseOR;
  } else if (data.code.front().data == "&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseAND;
  } else if (data.code.front().data == "^")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseXOR;
  } else if (data.code.front().data == "||")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LogicalOR;
  } else if (data.code.front().data == "&&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LogicalAND;
  } else if (data.code.front().data == "[")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Subscript;
  } else if (data.code.front().data == "=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::VariableAssignment;
  } else if (data.code.front().data == "+=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::AddEqual;
  } else if (data.code.front().data == "-=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::SubtractEqual;
  } else if (data.code.front().data == "*=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::MultiplyEqual;
  } else if (data.code.front().data == "/=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::DivideEqual;
  } else if (data.code.front().data == "%=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::ModuloEqual;
  } else if (data.code.front().data == "<<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LeftShiftEqual;
  } else if (data.code.front().data == ">>=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::RightShiftEqual;
  } else if (data.code.front().data == "|=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseOREqual;
  } else if (data.code.front().data == "&=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseANDEqual;
  } else if (data.code.front().data == "^=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseXOREqual;
  } else if (data.code.front().data == "==")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Equal;
  } else if (data.code.front().data == "!=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::NotEqual;
  } else if (data.code.front().data == ">")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Greater;
  } else if (data.code.front().data == "<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Lesser;
  } else if (data.code.front().data == ">=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::GreaterOrEqual;
  } else if (data.code.front().data == "<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LesserOrEqual;
  }

  data.code.pop_front();

  ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(Expression::parse(data, false));

  if (((BinaryOperator*)binary)->binaryType == BinaryOperator::BinaryType::Subscript)
  {
    ParseError::expect(data.code.front().data, "]");

    data.code.pop_front();

    return binary;
  }

  // this takes care of left-associativity and operator precedence
  if (((BinaryOperator*)binary)->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* operand = (BinaryOperator*)((BinaryOperator*)binary)->rightOperand.get();
    if (BinaryOperator::precedence[(uint8_t)operand->binaryType] < BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
    {
      ((BinaryOperator*)binary)->rightOperand.release();
      ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(operand->leftOperand.release());
      operand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    } else if (BinaryOperator::precedence[(uint8_t)operand->binaryType] == BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] && ((BinaryOperator*)binary)->binaryType != BinaryOperator::BinaryType::VariableAssignment)
    {
      BinaryOperator* bottomOperand = operand;
      while (
        bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->rightOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] ||
        bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
      {
        if (bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
        {
          bottomOperand = (BinaryOperator*)bottomOperand->rightOperand.get();
        } else
        {
          bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
        }
      }
      ((BinaryOperator*)binary)->rightOperand.release();
      ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
      bottomOperand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    }
  } else if (((BinaryOperator*)binary)->binaryType != BinaryOperator::BinaryType::VariableAssignment && ((BinaryOperator*)binary)->rightOperand->expressionType == Expression::ExpressionType::TernaryOperator)
  {
    TernaryOperator* operand = (TernaryOperator*)((BinaryOperator*)binary)->rightOperand.get();
    ((BinaryOperator*)binary)->rightOperand.release();
    ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(operand->condition.release());
    operand->condition = std::unique_ptr<Expression>(binary);
    binary = operand;
  }

  return binary;
}

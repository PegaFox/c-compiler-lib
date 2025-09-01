#include "binary_operator.hpp"

#include "parse_error.hpp"
#include "ternary_operator.hpp"

BinaryOperator::BinaryOperator()
{
  expressionType = ExpressionType::BinaryOperator;
}

Expression* BinaryOperator::parse(CommonParseData& data, Expression* leftOperand)
{
  Expression* binary;
  binary = data.program->arenaAlloc((BinaryOperator*)binary);

  ((BinaryOperator*)binary)->leftOperand = leftOperand;

  if (data.code.front().data == "+")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Add;
  } else if (data.code.front().data == "-")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Subtract;
  } else if (data.code.front().data == "*")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Multiply;
  } else if (data.code.front().data == "/")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Divide;
  } else if (data.code.front().data == "%")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Modulo;
  } else if (data.code.front().data == "<<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::LeftShift;
  } else if (data.code.front().data == ">>")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::RightShift;
  } else if (data.code.front().data == "|")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseOR;
  } else if (data.code.front().data == "&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseAND;
  } else if (data.code.front().data == "^")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseXOR;
  } else if (data.code.front().data == "||")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::LogicalOR;
  } else if (data.code.front().data == "&&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::LogicalAND;
  } else if (data.code.front().data == "[")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Subscript;
  } else if (data.code.front().data == "=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::VariableAssignment;
  } else if (data.code.front().data == "+=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::AddEqual;
  } else if (data.code.front().data == "-=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::SubtractEqual;
  } else if (data.code.front().data == "*=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::MultiplyEqual;
  } else if (data.code.front().data == "/=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::DivideEqual;
  } else if (data.code.front().data == "%=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::ModuloEqual;
  } else if (data.code.front().data == "<<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::LeftShiftEqual;
  } else if (data.code.front().data == ">>=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::RightShiftEqual;
  } else if (data.code.front().data == "|=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseOREqual;
  } else if (data.code.front().data == "&=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseANDEqual;
  } else if (data.code.front().data == "^=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::BitwiseXOREqual;
  } else if (data.code.front().data == "==")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Equal;
  } else if (data.code.front().data == "!=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::NotEqual;
  } else if (data.code.front().data == ">")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Greater;
  } else if (data.code.front().data == "<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::Lesser;
  } else if (data.code.front().data == ">=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::GreaterOrEqual;
  } else if (data.code.front().data == "<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::LesserOrEqual;
  } else if (data.code.front().data == ".")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::MemberAccess;
  } else if (data.code.front().data == "->")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryType::DereferenceMemberAccess;
  }

  data.code.pop_front();

  ((BinaryOperator*)binary)->rightOperand = Expression::parse(data, false);

  if (((BinaryOperator*)binary)->binaryType == BinaryType::Subscript)
  {
    ParseError::expect(data.code.front().data, "]");

    data.code.pop_front();

    return binary;
  }

  // this takes care of left-associativity and operator precedence
  if (((BinaryOperator*)binary)->rightOperand->expressionType == ExpressionType::BinaryOperator)
  {
    BinaryOperator* operand = (BinaryOperator*)((BinaryOperator*)binary)->rightOperand;
    if (precedence[(uint8_t)operand->binaryType] < precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
    {
      BinaryOperator* bottomOperand = operand;
      while (bottomOperand->leftOperand->expressionType == ExpressionType::BinaryOperator && precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand)->binaryType] < precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
      {
        bottomOperand = (BinaryOperator*)bottomOperand->leftOperand;
      }

      Expression* temp = ((BinaryOperator*)binary)->rightOperand;
      ((BinaryOperator*)binary)->rightOperand = bottomOperand->leftOperand;
      bottomOperand->leftOperand = binary;
      binary = temp;
    } else if (precedence[(uint8_t)operand->binaryType] == precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] && ((BinaryOperator*)binary)->binaryType != BinaryType::VariableAssignment)
    {
      BinaryOperator* bottomOperand = operand;
      while (
        bottomOperand->rightOperand->expressionType == ExpressionType::BinaryOperator &&
        precedence[(uint8_t)((BinaryOperator*)bottomOperand->rightOperand)->binaryType] ==
        precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] ||
        bottomOperand->leftOperand->expressionType == ExpressionType::BinaryOperator &&
        precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand)->binaryType] ==
        precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
      {
        if (bottomOperand->rightOperand->expressionType == ExpressionType::BinaryOperator)
        {
          bottomOperand = (BinaryOperator*)bottomOperand->rightOperand;
        } else
        {
          bottomOperand = (BinaryOperator*)bottomOperand->leftOperand;
        }
      }

      ((BinaryOperator*)binary)->rightOperand = bottomOperand->leftOperand;
      bottomOperand->leftOperand = binary;
      binary = operand;
    }
  } else if ((((BinaryOperator*)binary)->binaryType < BinaryType::VariableAssignment || ((BinaryOperator*)binary)->binaryType > BinaryType::BitwiseXOREqual) && ((BinaryOperator*)binary)->rightOperand->expressionType == ExpressionType::TernaryOperator)
  {
    TernaryOperator* operand = (TernaryOperator*)((BinaryOperator*)binary)->rightOperand;
    ((BinaryOperator*)binary)->rightOperand = operand->condition;
    operand->condition = binary;
    binary = operand;
  }

  return binary;
}

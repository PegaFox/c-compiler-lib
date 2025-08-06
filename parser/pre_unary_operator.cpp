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

  if ((data.code.front().type == Token::Keyword && data.code.front().data != "sizeof") || data.code.front().data == "(")
  {
    preUnary = TypeCast::parse(data);
  } else
  {
    preUnary = new PreUnaryOperator;

    if (data.code.front().data == "&")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::Address;
    } else if (data.code.front().data == "*")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::Dereference;
    } else if (data.code.front().data == "-")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::MathematicNegate;
    } else if (data.code.front().data == "~")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::BitwiseNOT;
    } else if (data.code.front().data == "!")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::LogicalNegate;
    } else if (data.code.front().data == "++")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::Increment;
    } else if (data.code.front().data == "--")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::Decrement;
    } else if (data.code.front().data == "sizeof")
    {
      ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryType::Sizeof;

      // Need to make sure child type cast doesn't take a child
      if ((++data.code.begin())->data == "(")
      {
        uint8_t depth = 0;
        for (std::list<Token>::iterator token = (++data.code.begin()); token != data.code.end(); token++)
        {
          if (token->data == "(")
          {
            depth++;
          } else if (token->data == ")")
          {
            depth--;
          }

          if (depth == 0)
          {
            data.code.insert(token, Token{Token::Other, ")"});
            break;
          }
        }

        data.code.insert(++data.code.begin(), Token{Token::Other, "("});
      }
    }

    if (((PreUnaryOperator*)preUnary)->preUnaryType != PreUnaryType::TypeCast)
    {
      data.code.pop_front();
    }
  }

  ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(Expression::parse(data, true));

  if (((PreUnaryOperator*)preUnary)->operand->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* bottomOperand = (BinaryOperator*)((PreUnaryOperator*)preUnary)->operand.get();
    if (BinaryOperator::precedence[(int)bottomOperand->binaryType] <= BinaryOperator::precedence[(int)BinaryOperator::BinaryType::Multiply])
    {
      while (
        BinaryOperator::precedence[(int)bottomOperand->binaryType] <= BinaryOperator::precedence[(int)BinaryOperator::BinaryType::Multiply] &&
        bottomOperand->leftOperand->expressionType == ExpressionType::BinaryOperator)
      {
        bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
      }
      Expression* operand = ((PreUnaryOperator*)preUnary)->operand.release();
      ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
      bottomOperand->leftOperand = std::unique_ptr<Expression>(preUnary);
      preUnary = operand;
    }
  }

  return preUnary;
}

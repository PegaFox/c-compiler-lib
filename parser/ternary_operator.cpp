#include "ternary_operator.hpp"

#include <iostream>

#include "parse_error.hpp"
#include "binary_operator.hpp"

TernaryOperator::TernaryOperator()
{
  expressionType = ExpressionType::TernaryOperator;
}

Expression* TernaryOperator::parse(CommonParseData& data, Expression* condition)
{
  Expression* ternary = new TernaryOperator;

  ((TernaryOperator*)ternary)->condition = std::unique_ptr<Expression>(condition);

  data.code.pop_front();

  ((TernaryOperator*)ternary)->trueOperand = std::unique_ptr<Expression>(Expression::parse(data));

  ParseError::expect(data.code.front().data, ":");

  data.code.pop_front();

  ((TernaryOperator*)ternary)->falseOperand = std::unique_ptr<Expression>(Expression::parse(data, false));

  if (
    ((TernaryOperator*)ternary)->falseOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
    ((BinaryOperator*)((TernaryOperator*)ternary)->falseOperand.get())->binaryType == BinaryOperator::BinaryType::VariableAssignment)
  {
    std::cout << "Parse error: cannot use ternary operator as lvalue\n";
    throw ParseError();
  }

  // this takes care of left-associativity and operator precedence
  if (((TernaryOperator*)ternary)->condition->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* operand = (BinaryOperator*)((TernaryOperator*)ternary)->condition.get();
    if (operand->binaryType == BinaryOperator::BinaryType::VariableAssignment)
    {
      ((TernaryOperator*)ternary)->condition.release();
      ((TernaryOperator*)ternary)->condition = std::unique_ptr<Expression>(operand->rightOperand.release());
      operand->rightOperand = std::unique_ptr<Expression>(ternary);
      ternary = operand;
    }/* else if (BinaryOperator::precedence[(uint8_t)operand->binaryType] == BinaryOperator::precedence[(uint8_t)binary->binaryType] && binary->binaryType != BinaryOperator::BinaryType::VariableAssignment)
    {
      BinaryOperator* bottomOperand = operand;
      while (
        bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->rightOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)binary->binaryType] ||
        bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)binary->binaryType])
      {
        if (bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
        {
          bottomOperand = (BinaryOperator*)bottomOperand->rightOperand.get();
        } else
        {
          bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
        }
      }
      binary->rightOperand.release();
      binary->rightOperand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
      bottomOperand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    }*/
  }

  return ternary;
}

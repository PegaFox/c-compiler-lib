#include "expression.hpp"

#include <iostream>

#include "parse_error.hpp"
#include "constant.hpp"
#include "sub_expression.hpp"
#include "pre_unary_operator.hpp"
#include "post_unary_operator.hpp"
#include "binary_operator.hpp"
#include "ternary_operator.hpp"
#include "function_call.hpp"
#include "variable_access.hpp"

extern bool optimize;

Expression::Expression()
{
  statementType = StatementType::Expression;
}

Expression* Expression::parse(std::list<Token>& code, Program& program, bool allowNullExpression)
{
  Expression* expression = nullptr;
  
  if (code.front().data == "(" && (code.begin()++)->type != Token::Keyword)
  {
    code.pop_front();

    expression = new SubExpression;
    
    ((SubExpression*)expression)->expression = std::unique_ptr<Expression>(Expression::parse(code, program, false));

    ParseError::expect(code.front().data, ")");
    code.pop_front();
  } else if (code.front().type == Token::Constant)
  {
    expression = Constant::parse(code);
  } else if (code.front().type == Token::Identifier)
  {
    if ((++code.begin())->data == "(")
    {
      expression = FunctionCall::parse(code, program);
    } else
    {
      expression = VariableAccess::parse(code);
    }
  } else if (
    code.front().type == Token::Keyword ||
    code.front().data == "(" && (code.begin()++)->type != Token::Keyword ||
    code.front().data == "&" ||
    code.front().data == "*" ||
    code.front().data == "-" ||
    code.front().data == "~" ||
    code.front().data == "!" ||
    code.front().data == "++" ||
    code.front().data == "--")
  {
    expression = PreUnaryOperator::parse(code, program);
    if (optimize && ((PreUnaryOperator*)expression)->operand.get()->expressionType == Expression::ExpressionType::Constant)
    {
      Constant* constant = (Constant*)(((PreUnaryOperator*)expression)->operand.get());
      switch (((PreUnaryOperator*)expression)->preUnaryType)
      {
        case PreUnaryOperator::PreUnaryType::MathematicNegate:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = -constant->value.unsignedChar;
          expression = constant;
          break;
        case PreUnaryOperator::PreUnaryType::LogicalNegate:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = !constant->value.unsignedChar;
          expression = constant;
          break;
        case PreUnaryOperator::PreUnaryType::BitwiseNOT:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = ~constant->value.unsignedChar;
          expression = constant;
          break;
        default:
          break;
      }
    }
  } else if (allowNullExpression && (code.front().data == ")" || code.front().data == ";"))
  {
    expression = new Expression;
    expression->expressionType = Expression::ExpressionType::Null;
  } else
  {
    std::cout << "Parse error: Expected an expression, received \"" << code.front().data << "\"\n";
    throw ParseError();
  }

  // separate prefixes and suffixes into seperate if statements
  if (code.front().data == ")")
  {
    return expression;
  } else if (
    code.front().data == "+" ||
    code.front().data == "-" ||
    code.front().data == "*" ||
    code.front().data == "/" ||
    code.front().data == "%" ||
    code.front().data == "<<" ||
    code.front().data == ">>" ||
    code.front().data == "|" ||
    code.front().data == "&" ||
    code.front().data == "||" ||
    code.front().data == "&&" ||
    code.front().data == "[" ||
    code.front().data == "+=" ||
    code.front().data == "-=" ||
    code.front().data == "*=" ||
    code.front().data == "/=" ||
    code.front().data == "%=" ||
    code.front().data == "<<=" ||
    code.front().data == ">>=" ||
    code.front().data == "|=" ||
    code.front().data == "&=" ||
    code.front().data == "^=" ||
    code.front().data == "=" ||
    code.front().data == ">" ||
    code.front().data == "<" ||
    code.front().data == ">=" ||
    code.front().data == "<=" ||
    code.front().data == "==" ||
    code.front().data == "!=" ||
    code.front().data == "^")
  {
    expression = BinaryOperator::parse(code, program, expression);

  } else if (
    code.front().data == "++" ||
    code.front().data == "--")
  {
    expression = PostUnaryOperator::parse(code, expression);
  } else if (code.front().data == "?")
  {
    expression = TernaryOperator::parse(code, program, expression);
  }

  return expression;
}

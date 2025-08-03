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

Expression* Expression::parse(CommonParseData& data, bool allowNullExpression)
{
  Expression* expression = nullptr;
  
  if (data.code.front().data == "(" && (data.code.begin()++)->type != Token::Keyword)
  {
    data.code.pop_front();

    expression = new SubExpression;
    
    ((SubExpression*)expression)->expression = std::unique_ptr<Expression>(Expression::parse(data, false));

    ParseError::expect(data.code.front().data, ")");
    data.code.pop_front();
  } else if (data.code.front().type == Token::Constant)
  {
    expression = Constant::parse(data);
  } else if (data.code.front().type == Token::Identifier)
  {
    if ((++data.code.begin())->data == "(")
    {
      expression = FunctionCall::parse(data);
    } else
    {
      expression = VariableAccess::parse(data);
    }
  } else if (
    data.code.front().type == Token::Keyword ||
    data.code.front().data == "(" && (data.code.begin()++)->type != Token::Keyword ||
    data.code.front().data == "&" ||
    data.code.front().data == "*" ||
    data.code.front().data == "-" ||
    data.code.front().data == "~" ||
    data.code.front().data == "!" ||
    data.code.front().data == "++" ||
    data.code.front().data == "--")
  {
    expression = PreUnaryOperator::parse(data);
  } else if (allowNullExpression && (data.code.front().data == ")" || data.code.front().data == ";"))
  {
    expression = new Expression;
    expression->expressionType = Expression::ExpressionType::Null;
  } else
  {
    std::cout << "Parse error: Expected an expression, received \"" << data.code.front().data << "\"\n";
    throw ParseError();
  }

  // separate prefixes and suffixes into seperate if statements
  if (data.code.front().data == ")")
  {
    return expression;
  } else if (
    data.code.front().data == "+" ||
    data.code.front().data == "-" ||
    data.code.front().data == "*" ||
    data.code.front().data == "/" ||
    data.code.front().data == "%" ||
    data.code.front().data == "<<" ||
    data.code.front().data == ">>" ||
    data.code.front().data == "|" ||
    data.code.front().data == "&" ||
    data.code.front().data == "||" ||
    data.code.front().data == "&&" ||
    data.code.front().data == "[" ||
    data.code.front().data == "+=" ||
    data.code.front().data == "-=" ||
    data.code.front().data == "*=" ||
    data.code.front().data == "/=" ||
    data.code.front().data == "%=" ||
    data.code.front().data == "<<=" ||
    data.code.front().data == ">>=" ||
    data.code.front().data == "|=" ||
    data.code.front().data == "&=" ||
    data.code.front().data == "^=" ||
    data.code.front().data == "=" ||
    data.code.front().data == ">" ||
    data.code.front().data == "<" ||
    data.code.front().data == ">=" ||
    data.code.front().data == "<=" ||
    data.code.front().data == "==" ||
    data.code.front().data == "!=" ||
    data.code.front().data == "^")
  {
    expression = BinaryOperator::parse(data, expression);

  } else if (
    data.code.front().data == "++" ||
    data.code.front().data == "--")
  {
    expression = PostUnaryOperator::parse(data, expression);
  } else if (data.code.front().data == "?")
  {
    expression = TernaryOperator::parse(data, expression);
  }

  return expression;
}

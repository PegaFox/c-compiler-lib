#ifndef PF_PARSER_EXPRESSION_HPP
#define PF_PARSER_EXPRESSION_HPP

#include "statement.hpp"

struct Expression: public Statement
{
  enum class ExpressionType
  {
    Undefined, // if a node has this type, something went wrong
    Null,
    Constant,
    FunctionCall,
    VariableAccess,
    SubExpression,
    PreUnaryOperator,
    PostUnaryOperator,
    BinaryOperator,
    TernaryOperator
  } expressionType;

  Expression();
};

#endif // PF_PARSER_EXPRESSION_HPP

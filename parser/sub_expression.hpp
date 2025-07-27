#ifndef PF_PARSER_SUB_EXPRESSION_HPP
#define PF_PARSER_SUB_EXPRESSION_HPP

#include <memory>

#include "expression.hpp"

struct SubExpression: public Expression
{
  std::unique_ptr<Expression> expression;

  SubExpression();
};

#endif // PF_PARSER_SUB_EXPRESSION_HPP

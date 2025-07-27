#ifndef PF_PARSER_TERNARY_OPERATOR_HPP
#define PF_PARSER_TERNARY_OPERATOR_HPP

#include <memory>

#include "expression.hpp"

struct TernaryOperator: public Expression
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Expression> trueOperand;

  std::unique_ptr<Expression> falseOperand;

  TernaryOperator();
};

#endif // PF_PARSER_TERNARY_OPERATOR_HPP

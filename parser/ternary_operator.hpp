#ifndef PF_PARSER_TERNARY_OPERATOR_HPP
#define PF_PARSER_TERNARY_OPERATOR_HPP

#include <memory>

#include "expression.hpp"

struct TernaryOperator: public Expression
{
  Expression* condition = nullptr;

  Expression* trueOperand = nullptr;

  Expression* falseOperand = nullptr;

  TernaryOperator();

  static Expression* parse(CommonParseData& data, Expression* condition);
};

#endif // PF_PARSER_TERNARY_OPERATOR_HPP

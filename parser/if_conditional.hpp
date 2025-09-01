#ifndef PF_PARSER_IF_CONDITIONAL_HPP
#define PF_PARSER_IF_CONDITIONAL_HPP

#include <memory>

#include "expression.hpp"

struct IfConditional: public Statement
{
  Expression* condition = nullptr;

  Statement* body = nullptr;

  // associated else statement
  Statement* elseStatement = nullptr;

  IfConditional();

  static IfConditional* parse(CommonParseData& data);
};

#endif // PF_PARSER_IF_CONDITIONAL_HPP

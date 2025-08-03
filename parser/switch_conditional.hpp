#ifndef PF_PARSER_SWITCH_CONDITIONAL_HPP
#define PF_PARSER_SWITCH_CONDITIONAL_HPP

#include <memory>

#include "expression.hpp"

struct SwitchConditional: public Statement
{
  std::unique_ptr<Expression> value;

  std::unique_ptr<Statement> body;

  SwitchConditional();

  static SwitchConditional* parse(CommonParseData& data);
};

#endif // PF_PARSER_SWITCH_CONDITIONAL_HPP

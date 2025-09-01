#ifndef PF_PARSER_SWITCH_CASE_HPP
#define PF_PARSER_SWITCH_CASE_HPP

#include <memory>

#include "expression.hpp"

struct SwitchCase: public Statement
{
  Expression* requirement = nullptr;

  SwitchCase();

  static SwitchCase* parse(CommonParseData& data);
};

#endif // PF_PARSER_SWITCH_CASE_HPP

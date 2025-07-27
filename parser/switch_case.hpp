#ifndef PF_PARSER_SWITCH_CASE_HPP
#define PF_PARSER_SWITCH_CASE_HPP

#include <memory>

#include "expression.hpp"

struct SwitchCase: public Statement
{
  std::unique_ptr<Expression> requirement;

  SwitchCase();

  static SwitchCase* parse(std::list<Token>& code);
};

#endif // PF_PARSER_SWITCH_CASE_HPP

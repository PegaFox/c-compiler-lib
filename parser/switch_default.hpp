#ifndef PF_PARSER_SWITCH_DEFAULT_HPP
#define PF_PARSER_SWITCH_DEFAULT_HPP

#include "statement.hpp"

struct SwitchDefault: public Statement
{
  SwitchDefault();

  static SwitchDefault* parse(std::list<Token>& code);
};

#endif // PF_PARSER_SWITCH_DEFAULT_HPP

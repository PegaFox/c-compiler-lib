#ifndef PF_PARSER_SWITCH_DEFAULT_HPP
#define PF_PARSER_SWITCH_DEFAULT_HPP

#include "statement.hpp"

struct SwitchDefault: public Statement
{
  SwitchDefault();

  static SwitchDefault* parse(CommonParseData& data);
};

#endif // PF_PARSER_SWITCH_DEFAULT_HPP

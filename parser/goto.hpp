#ifndef PF_PARSER_GOTO_HPP
#define PF_PARSER_GOTO_HPP

#include <string>

#include "statement.hpp"

struct Goto: public Statement
{
  std::string_view label;

  Goto();

  static Goto* parse(CommonParseData& data);
};

#endif // PF_PARSER_GOTO_HPP

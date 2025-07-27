#ifndef PF_PARSER_GOTO_HPP
#define PF_PARSER_GOTO_HPP

#include <string>

#include "statement.hpp"

struct Goto: public Statement
{
  std::string label;

  Goto();
};

#endif // PF_PARSER_GOTO_HPP

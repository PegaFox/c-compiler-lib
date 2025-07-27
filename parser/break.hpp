#ifndef PF_PARSER_BREAK_HPP
#define PF_PARSER_BREAK_HPP

#include "statement.hpp"

struct Break: public Statement
{
  Break();

  static Break* parse(std::list<Token>& code);
};

#endif // PF_PARSER_BREAK_HPP

#ifndef PF_PARSER_CONTINUE_HPP
#define PF_PARSER_CONTINUE_HPP

#include "statement.hpp"

struct Continue: public Statement
{
  Continue();

  static Continue* parse(CommonParseData& data);
};

#endif // PF_PARSER_CONTINUE_HPP

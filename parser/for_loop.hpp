#ifndef PF_PARSER_FOR_LOOP_HPP
#define PF_PARSER_FOR_LOOP_HPP

#include <memory>

#include "expression.hpp"

struct ForLoop: public Statement
{
  Statement* initialization = nullptr;

  Expression* condition = nullptr;

  Expression* update = nullptr;

  Statement* body = nullptr;

  ForLoop();

  static ForLoop* parse(CommonParseData& data);
};

#endif // PF_PARSER_FOR_LOOP_HPP

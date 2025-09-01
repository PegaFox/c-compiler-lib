#ifndef PF_PARSER_DO_WHIL_LLOOP_HPP
#define PF_PARSER_DO_WHIL_LLOOP_HPP

#include <memory>

#include "expression.hpp"

struct DoWhileLoop: public Statement
{
  Expression* condition = nullptr;

  Statement* body = nullptr;

  DoWhileLoop();

  static DoWhileLoop* parse(CommonParseData& data);
};

#endif // PF_PARSER_DO_WHIL_LLOOP_HPP

#ifndef PF_PARSER_WHILE_LOOP_HPP
#define PF_PARSER_WHILE_LOOP_HPP

#include <memory>

#include "expression.hpp"

struct WhileLoop: public Statement
{
  Expression* condition = nullptr;

  Statement* body = nullptr;

  WhileLoop();

  static WhileLoop* parse(CommonParseData& data);
};

#endif // PF_PARSER_WHILE_LOOP_HPP

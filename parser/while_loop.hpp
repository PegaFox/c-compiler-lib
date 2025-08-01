#ifndef PF_PARSER_WHILE_LOOP_HPP
#define PF_PARSER_WHILE_LOOP_HPP

#include <memory>

#include "expression.hpp"

struct WhileLoop: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  WhileLoop();

  static WhileLoop* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_WHILE_LOOP_HPP

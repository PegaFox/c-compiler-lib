#ifndef PF_PARSER_DO_WHIL_LLOOP_HPP
#define PF_PARSER_DO_WHIL_LLOOP_HPP

#include <memory>

#include "expression.hpp"

struct DoWhileLoop: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  DoWhileLoop();

  static DoWhileLoop* parse(std::list<Token>& code);
};

#endif // PF_PARSER_DO_WHIL_LLOOP_HPP

#ifndef PF_PARSER_IF_CONDITIONAL_HPP
#define PF_PARSER_IF_CONDITIONAL_HPP

#include <memory>

#include "expression.hpp"

struct IfConditional: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  // associated else statement
  std::unique_ptr<Statement> elseStatement;

  IfConditional();

  static IfConditional* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_IF_CONDITIONAL_HPP

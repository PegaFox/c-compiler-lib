#ifndef PF_PARSER_FOR_LOOP_HPP
#define PF_PARSER_FOR_LOOP_HPP

#include <memory>

#include "expression.hpp"

struct ForLoop: public Statement
{
  std::unique_ptr<Statement> initialization;

  std::unique_ptr<Expression> condition;

  std::unique_ptr<Expression> update;

  std::unique_ptr<Statement> body;

  ForLoop();

  static ForLoop* parse(CommonParseData& data);
};

#endif // PF_PARSER_FOR_LOOP_HPP

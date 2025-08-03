#ifndef PF_PARSER_COMPOUND_STATEMENT_HPP
#define PF_PARSER_COMPOUND_STATEMENT_HPP

#include <vector>
#include <memory>

#include "statement.hpp"

struct CompoundStatement: public Statement
{
  std::vector<std::unique_ptr<Statement>> body;

  CompoundStatement();

  static CompoundStatement* parse(CommonParseData& data);
};

#endif // PF_PARSER_COMPOUND_STATEMENT_HPP

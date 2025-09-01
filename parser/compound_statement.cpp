#include "compound_statement.hpp"

#include <iostream>

#include "parse_error.hpp"

CompoundStatement::CompoundStatement()
{
  statementType = StatementType::CompoundStatement;
}

CompoundStatement* CompoundStatement::parse(CommonParseData& data)
{
  CompoundStatement* compoundStatement;
  compoundStatement = data.program->arenaAlloc(compoundStatement);

  ParseError::expect(data.code.front().data, "{");
  data.code.pop_front();

  while (data.code.front().data != "}")
  {
    compoundStatement->body.emplace_back(Statement::parse(data, true));

    if (data.code.empty())
    {
      std::cout << "Parse error: Unexpected End of File\n";
      throw ParseError();
    }
  }
  data.code.pop_front();

  return compoundStatement;
}

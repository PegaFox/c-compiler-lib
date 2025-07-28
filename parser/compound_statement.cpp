#include "compound_statement.hpp"

#include <iostream>

#include "parse_error.hpp"

CompoundStatement::CompoundStatement()
{
  statementType = StatementType::CompoundStatement;
}

CompoundStatement* CompoundStatement::parse(std::list<Token>& code)
{
  CompoundStatement* compoundStatement = new CompoundStatement;

  ParseError::expect(code.front().data, "{");
  code.pop_front();

  while (code.front().data != "}")
  {
    compoundStatement->body.emplace_back(Statement::parse(code, true));

    if (code.empty())
    {
      std::cout << "Parse error: Unexpected End of File\n";
      throw ParseError();
    }
  }
  code.pop_front();

  return compoundStatement;
}

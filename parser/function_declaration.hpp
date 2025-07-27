#ifndef PF_PARSER_FUNCTION_DECLARATION_HPP
#define PF_PARSER_FUNCTION_DECLARATION_HPP

#include "variable_declaration.hpp"
#include "compound_statement.hpp"

struct FunctionDeclaration: public Statement
{
  std::unique_ptr<DataType> returnType;

  std::string identifier;

  std::vector<std::unique_ptr<VariableDeclaration>> parameters;

  std::unique_ptr<CompoundStatement> body;

  FunctionDeclaration();

  static FunctionDeclaration* parse(std::list<Token>& code);
};

#endif // PF_PARSER_FUNCTION_DECLARATION_HPP

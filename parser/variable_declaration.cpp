#include "variable_declaration.hpp"

#include "parse_error.hpp"

VariableDeclaration::VariableDeclaration()
{
  statementType = StatementType::VariableDeclaration;
}

VariableDeclaration* VariableDeclaration::parse(std::list<Token>& code)
{
  VariableDeclaration* variableDeclaration = new VariableDeclaration;

  variableDeclaration->dataType = std::unique_ptr<DataType>(DataType::parse(code));

  ParseError::expect(code.front(), Token::Identifier);
  variableDeclaration->identifier = code.front().data;
  code.pop_front();

  if (code.front().data == "=")
  {
    code.pop_front();
    variableDeclaration->value = std::unique_ptr<Expression>(Expression::parse(code, false));
  }

  //ParseError::expect(code.front().data, {";", ",", ")"});
  //code.pop_front();

  return variableDeclaration;
}

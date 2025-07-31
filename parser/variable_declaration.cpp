#include "variable_declaration.hpp"

#include "parse_error.hpp"

VariableDeclaration::VariableDeclaration()
{
  statementType = StatementType::VariableDeclaration;
}

VariableDeclaration* VariableDeclaration::parse(std::list<Token>& code, Program& program)
{
  VariableDeclaration* variableDeclaration = new VariableDeclaration;

  variableDeclaration->dataType = std::unique_ptr<DataType>(DataType::parse(code, program));

  if (code.front().type == Token::Identifier)
  {
    variableDeclaration->identifier = code.front().data;
    code.pop_front();

    if (code.front().data == "=")
    {
      code.pop_front();
      variableDeclaration->value = std::unique_ptr<Expression>(Expression::parse(code, program, false));
    } else if (variableDeclaration->dataType->isTypedef)
    {
      program.typedefs[variableDeclaration->identifier] = std::unique_ptr<DataType>(variableDeclaration->dataType.get());
    }

    //ParseError::expect(code.front().data, {";", ",", ")"});
    //code.pop_front();
  }

  return variableDeclaration;
}

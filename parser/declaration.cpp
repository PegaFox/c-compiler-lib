#include "declaration.hpp"

#include "parse_error.hpp"

Declaration::Declaration()
{
  statementType = StatementType::Declaration;
}

Declaration* Declaration::parse(std::list<Token>& code, Program& program)
{
  Declaration* declaration = new Declaration;

  if (code.front().data == "typedef")
  {
    declaration->isTypedef = true;
    code.pop_front();
  } else
  {
    declaration->isTypedef = false;
  }

  declaration->dataType = std::unique_ptr<DataType>(DataType::parse(code, program));

  if (code.front().type == Token::Identifier)
  {
    declaration->identifier = code.front().data;
    code.pop_front();

    if (code.front().data == "=")
    {
      code.pop_front();
      declaration->value = std::unique_ptr<Expression>(Expression::parse(code, program, false));
    } else if (declaration->isTypedef)
    {
      program.typedefs[declaration->identifier] = std::unique_ptr<DataType>(declaration->dataType.get());
    }

    //ParseError::expect(code.front().data, {";", ",", ")"});
    //code.pop_front();
  }

  return declaration;
}

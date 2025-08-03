#include "declaration.hpp"

#include "expression.hpp"

Declaration::Declaration()
{
  statementType = StatementType::Declaration;
}

Declaration* Declaration::parse(CommonParseData& data)
{
  Declaration* declaration = new Declaration;

  if (data.code.front().data == "typedef")
  {
    declaration->isTypedef = true;
    data.code.pop_front();
  } else
  {
    declaration->isTypedef = false;
  }

  declaration->dataType = std::unique_ptr<DataType>(DataType::parse(data));

  if (data.code.front().type == Token::Identifier)
  {
    declaration->identifier = data.code.front().data;
    data.code.pop_front();

    if (data.code.front().data == "=")
    {
      data.code.pop_front();
      declaration->value = std::unique_ptr<Expression>(Expression::parse(data, false));
    } else if (declaration->isTypedef)
    {
      data.program->typedefs[declaration->identifier] = std::unique_ptr<DataType>(declaration->dataType.get());
    }

    //ParseError::expect(code.front().data, {";", ",", ")"});
    //code.pop_front();
  }

  return declaration;
}

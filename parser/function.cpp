#include "function.hpp"

Function::Function()
{
  generalType = GeneralType::Function;
}

/*Function* Function::parse(std::list<Token>& code, Program& program)
{
  Function* functionDeclaration = new Function;

  functionDeclaration->returnType = std::unique_ptr<DataType>(DataType::parse(code, program));

  ParseError::expect(code.front(), Token::Identifier);
  functionDeclaration->identifier = code.front().data;
  code.pop_front();

  ParseError::expect(code.front().data, "(");
  code.pop_front();

  if (code.front().data == "void")
  {
    code.pop_front();
  }

  while (code.front().data != ")")
  {
    if (code.front().data == ",")
    {
      code.pop_front();
    }

    functionDeclaration->parameters.emplace_back(std::unique_ptr<VariableDeclaration>(VariableDeclaration::parse(code, program)));
  }

  code.pop_front();

  ParseError::expect(code.front().data, {"{", ";"});
  if (code.front().data == "{")
  {
    functionDeclaration->body = std::unique_ptr<CompoundStatement>(CompoundStatement::parse(code, program));
  } else
  {
    code.pop_front();
  }

  return functionDeclaration;
}*/

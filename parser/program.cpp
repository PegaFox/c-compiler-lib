#include "program.hpp"

#include <iostream>

#include "parse_error.hpp"
#include "function.hpp"

Program::Program()
{
  nodeType = NodeType::Program;
}

Program::Program(const std::list<Token>& code)
{
  nodeType = NodeType::Program;

  parse(code);
}

void Program::parse(std::list<Token> code)
{
  while (!code.empty()) {
    for (std::list<Token>::iterator i = code.begin(); i != code.end(); i++)
    {
      switch (i->type)
      {
        case Token::Identifier:
          if (((++i)--)->data == "(")
          {
            nodes.emplace_back(Function::parse(code, *this));
            i = code.end();
          }
          break;
        case Token::Keyword:
          if (i->data == "enum")
          {
            parseEnum(code);
            ParseError::expect(code.front().data, ";");
            code.pop_front();
            i = code.end();
          }
          break;
        case Token::Operator:
          if (i->data == "=")
          {
            nodes.emplace_back(Declaration::parse(code, *this));
            ParseError::expect(code.front().data, ";");
            code.pop_front();
            i = code.end();
          }
          break;
        case Token::Other:
          if (i->data == ";")
          {
            nodes.emplace_back(Declaration::parse(code, *this));
            ParseError::expect(code.front().data, ";");
            code.pop_front();
            i = code.end();
          }
          break;
        /*default:
          std::cout << "Parse error: Expected DataType, received " << Token::typeStrings[i->type] << "\n";
          break;*/
      }
    }
  }
}

void Program::parseEnum(std::list<Token>& code)
{
  ParseError::expect(code.front().data, "enum");
  code.pop_front();

  ParseError::expect(code.front(), Token::Identifier);
  enumTypes.emplace(code.front().data);
  code.pop_front();

  ParseError::expect(code.front().data, "{");
  code.pop_front();

  ENUM_TYPE value = 0;
  while (code.front().data != "}")
  {
    ParseError::expect(code.front(), Token::Identifier);
    if (enums.contains(code.front().data))
    {
      std::cout << "Parse error: Enum value \"" << code.front().data << "\" already declared\n";
      throw ParseError();
    }

    std::string name = code.front().data;
    code.pop_front();

    if (code.front().data == "=")
    {
      code.pop_front();

      value = std::stoi(code.front().data);
      code.pop_front();
    }
    
    enums[name] = value;

    value++;

    if (code.front().data == ",")
    {
      code.pop_front();
    }
  }
  code.pop_front();
}

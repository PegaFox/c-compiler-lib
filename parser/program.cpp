#include "program.hpp"

#include <iostream>

#include "parse_error.hpp"
#include "declaration.hpp"

Program::Program()
{
  nodeType = NodeType::Program;
}

Program::Program(const std::list<Token>& code, const Compiler::TypeSizes& typeSizes)
{
  nodeType = NodeType::Program;

  parse(code, typeSizes);
}

void Program::parse(std::list<Token> code, const Compiler::TypeSizes& typeSizes)
{
  CommonParseData data{code, this, typeSizes};

  while (!data.code.empty()) {
    for (std::list<Token>::iterator i = data.code.begin(); i != data.code.end(); i++)
    {
      switch (i->type)
      {
        case Token::Identifier:
          if (((++i)--)->data == "(")
          {
            nodes.emplace_back(Declaration::parse(data));
            if (!data.code.empty() && data.code.front().data == ";")
            {
              data.code.pop_front();
            }
            i = data.code.end();
          }
          break;
        case Token::Keyword:
          if (i->data == "enum")
          {
            parseEnum(data);
            ParseError::expect(data.code.front().data, ";");
            data.code.pop_front();
            i = data.code.end();
          }
          break;
        case Token::Operator:
          if (i->data == "=")
          {
            nodes.emplace_back(Declaration::parse(data));
            ParseError::expect(data.code.front().data, ";");
            data.code.pop_front();
            i = data.code.end();
          }
          break;
        case Token::Other:
          if (i->data == ";")
          {
            nodes.emplace_back(Declaration::parse(data));
            ParseError::expect(data.code.front().data, ";");
            data.code.pop_front();
            i = data.code.end();
          }
          break;
        /*default:
          std::cout << "Parse error: Expected DataType, received " << Token::typeStrings[i->type] << "\n";
          break;*/
      }
    }
  }
}

void Program::parseEnum(CommonParseData& data)
{
  ParseError::expect(data.code.front().data, "enum");
  data.code.pop_front();

  ParseError::expect(data.code.front(), Token::Identifier);
  enumTypes.emplace(data.code.front().data);
  data.code.pop_front();

  ParseError::expect(data.code.front().data, "{");
  data.code.pop_front();

  ENUM_TYPE value = 0;
  while (data.code.front().data != "}")
  {
    ParseError::expect(data.code.front(), Token::Identifier);
    if (enums.contains(data.code.front().data))
    {
      std::cout << "Parse error: Enum value \"" << data.code.front().data << "\" already declared\n";
      throw ParseError();
    }

    std::string name = data.code.front().data;
    data.code.pop_front();

    if (data.code.front().data == "=")
    {
      data.code.pop_front();

      value = std::stoi(data.code.front().data);
      data.code.pop_front();
    }
    
    enums[name] = value;

    value++;

    if (data.code.front().data == ",")
    {
      data.code.pop_front();
    }
  }
  data.code.pop_front();
}

#include "data_type.hpp"

#include "parse_error.hpp"
#include "primitive_type.hpp"
#include "pointer.hpp"
#include "function.hpp"
#include "array.hpp"
#include "struct.hpp"


DataType::DataType()
{
  nodeType = NodeType::DataType;
}

DataType* DataType::parse(std::list<Token>& code, Program& program, DataType::Linkage defaultLinkage)
{
  DataType* dataType = nullptr;

  if (program.typedefs.contains(code.front().data))
  {
    dataType = program.typedefs[code.front().data].get();
    code.pop_front();
    return dataType;
  }

  // scan ahead to check if the datatype is primitive or a struct
  for (std::list<Token>::iterator i = code.begin(); i != code.end(); i++)
  {
    if (i->type == Token::Keyword)
    {
      if (i->data == "struct")
      {
        dataType = new Struct;
      }
    } else if (i->data == "(")
    {
      dataType = new Function;
    } else
    {
      break;
    }
  }

  if (dataType == nullptr)
  {
    dataType = new PrimitiveType;
  }

  dataType->linkage = defaultLinkage;
  bool changed = true;
  while (changed)
  {
    changed = false;

    if (code.front().data == "const")
    {
      changed = true;
      dataType->isConst = true;
      code.pop_front();
    }

    if (code.front().data == "volatile")
    {
      changed = true;
      dataType->isVolatile = true;
      code.pop_front();
    }

    if (code.front().data == "typedef")
    {
      changed = true;
      dataType->isTypedef = true;
      code.pop_front();
    }

    if (code.front().data == "static")
    {
      changed = true;
      dataType->linkage = DataType::Linkage::Internal;
      code.pop_front();
    } else if (code.front().data == "extern")
    {
      changed = true;
      dataType->linkage = DataType::Linkage::External;
      code.pop_front();
    }
  }

  if (dataType->generalType == DataType::GeneralType::Struct && code.front().data == "struct")
  {
    Struct* structure = (Struct*)dataType;
    code.pop_front();

    ParseError::expect(code.front(), Token::Identifier);
    structure->identifier = code.front().data;
    code.pop_front();

    if (code.front().data == "{")
    {
      code.pop_front();
      while (code.front().data != "}")
      {
        structure->members.emplace_back(Declaration::parse(code, program), -1);

        if (code.front().data == ":")
        {
          code.pop_front();

          ParseError::expect(code.front(), Token::Constant);
          structure->members.back().second = std::stoi(code.front().data);
          code.pop_front();
        }

        ParseError::expect(code.front().data, ";");
        code.pop_front();
      }
      code.pop_front();
    }

  } else if (dataType->generalType == DataType::GeneralType::Function)
  {
    Function* function = (Function*)dataType;

    std::list<Token> buffer;
    // we only use the section of the code with the return type so it doesn't recursively create functions
    while (code.front().data != "(")
    {
      buffer.push_back(code.front());
      code.pop_front();
    }
    // need to add a dummy token to the end to avoid segfault when checking first element of list
    buffer.push_back(Token{Token::Other, ";"});

    function->returnType = std::unique_ptr<DataType>(DataType::parse(buffer, program));

    buffer.pop_back();
    while (!buffer.empty())
    {
      code.push_front(buffer.back());
      buffer.pop_back();
    }

    if (code.front().data == "(")
    {
      code.pop_front();
    }
    
    while (code.front().data != "(")
    {
      // this effectively removes the closing parentheses for function pointers
      if (code.front().data != ")")
      {
        buffer.push_front(code.front());
      }

      code.pop_front();
    }
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

      function->parameters.emplace_back(std::unique_ptr<Declaration>(Declaration::parse(code, program)));
    }
    code.pop_front();

    while (!buffer.empty())
    {
      code.push_front(buffer.front());
      buffer.pop_front();
    }
  } else if (dataType->generalType == DataType::GeneralType::PrimitiveType)
  {
    PrimitiveType* primitiveType = (PrimitiveType*)dataType;
    
    switch (ParseError::expect(code.front().data, {"void", "signed", "unsigned", "short", "long", "char", "int", "float", "double"})) {
      case 0:
        primitiveType->type = PrimitiveType::Type::Void;
        code.pop_front();
        break;
      case 1:
        code.pop_front();

        if (code.front().data == "short")
        {
          primitiveType->type = PrimitiveType::Type::SignedShort;
          code.pop_front();
          if (code.front().data == "int")
          {
            code.pop_front();
          }
        } else if (code.front().data == "long")
        {
          code.pop_front();
          if (code.front().data == "long")
          {
            primitiveType->type = PrimitiveType::Type::SignedLongLong;
            code.pop_front();
          } else
          {
            primitiveType->type = PrimitiveType::Type::SignedLong;
          }
          if (code.front().data == "int")
          {
            code.pop_front();
          }
        } else if (code.front().data == "char")
        {
          primitiveType->type = PrimitiveType::Type::SignedChar;
          code.pop_front();
        } else
        {
          primitiveType->type = PrimitiveType::Type::SignedInt;

          if (code.front().data == "int")
          {
            code.pop_front();
          }
        }
        break;
      case 2:
        code.pop_front();

        if (code.front().data == "short")
        {
          primitiveType->type = PrimitiveType::Type::UnsignedShort;
          code.pop_front();
          if (code.front().data == "int")
          {
            code.pop_front();
          }
        } else if (code.front().data == "long")
        {
          code.pop_front();
          if (code.front().data == "long")
          {
            primitiveType->type = PrimitiveType::Type::UnsignedLongLong;
            code.pop_front();
          } else
          {
            primitiveType->type = PrimitiveType::Type::UnsignedLong;
          }
          if (code.front().data == "int")
          {
            code.pop_front();
          }
        } else if (code.front().data == "char")
        {
          primitiveType->type = PrimitiveType::Type::UnsignedChar;
          code.pop_front();
        } else
        {
          primitiveType->type = PrimitiveType::Type::UnsignedInt;

          if (code.front().data == "int")
          {
            code.pop_front();
          }
        }
        break;
      case 3:
        primitiveType->type = PrimitiveType::Type::SignedShort;
        code.pop_front();
        if (code.front().data == "int")
        {
          code.pop_front();
        }
        break;
      case 4:
        code.pop_front();
        if (code.front().data == "long")
        {
          primitiveType->type = PrimitiveType::Type::SignedLongLong;
          code.pop_front();
        } else
        {
          primitiveType->type = PrimitiveType::Type::SignedLong;
        }
        if (code.front().data == "int")
        {
          code.pop_front();
        }
        break;
      case 5:
        primitiveType->type = PrimitiveType::Type::SignedChar;
        code.pop_front();
        break;
      case 6:
        primitiveType->type = PrimitiveType::Type::SignedInt;
        code.pop_front();
        break;
      case 7:
        primitiveType->type = PrimitiveType::Type::Float;
        code.pop_front();
        break;
      case 8:
        primitiveType->type = PrimitiveType::Type::Double;
        code.pop_front();
        break;
      default:

        break;
    }
    
    /*if (code.front().data == "static")
    {
      primitiveType->linkage = DataType::Linkage::Internal;
      code.pop_front();
    } else if (code.front().data == "extern")
    {
      primitiveType->linkage = DataType::Linkage::External;
      code.pop_front();
    }*/
  }

  while (code.front().data == "*")
  {
    Pointer* pointer = new Pointer;
    pointer->dataType = std::unique_ptr<DataType>(dataType);
    dataType = pointer;
    code.pop_front();
  }

  Token variableIdentifier;
  if ((code.front().type == Token::Identifier && (++code.begin())->data == "["))
  {
    variableIdentifier = code.front();
    code.pop_front();
  }

  while (code.front().data == "[")
  {
    Array* array = new Array;
    array->dataType = std::unique_ptr<DataType>(dataType);
    dataType = array;

    code.pop_front();
    array->size = std::unique_ptr<Expression>(Expression::parse(code, program));

    ParseError::expect(code.front().data, "]");
    code.pop_front();
  }

  if (variableIdentifier.type == Token::Identifier)
  {
    code.push_front(variableIdentifier);
  }

  return dataType;
}

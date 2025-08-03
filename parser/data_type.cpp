#include "data_type.hpp"

#include <iostream>

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

DataType* DataType::parse(CommonParseData& data, DataType::Linkage defaultLinkage)
{
  DataType* dataType = nullptr;

  if (data.program->typedefs.contains(data.code.front().data))
  {
    dataType = data.program->typedefs[data.code.front().data].get();
    data.code.pop_front();
    return dataType;
  }

  // scan ahead to check if the datatype is primitive or a struct
  for (std::list<Token>::iterator i = data.code.begin(); i != data.code.end(); i++)
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

    if (data.code.front().data == "const")
    {
      changed = true;
      dataType->isConst = true;
      data.code.pop_front();
    }

    if (data.code.front().data == "volatile")
    {
      changed = true;
      dataType->isVolatile = true;
      data.code.pop_front();
    }

    if (data.code.front().data == "static")
    {
      changed = true;
      dataType->linkage = DataType::Linkage::Internal;
      data.code.pop_front();
    } else if (data.code.front().data == "extern")
    {
      changed = true;
      dataType->linkage = DataType::Linkage::External;
      data.code.pop_front();
    }
  }

  if (dataType->generalType == DataType::GeneralType::Struct && data.code.front().data == "struct")
  {
    Struct* structure = (Struct*)dataType;
    data.code.pop_front();

    ParseError::expect(data.code.front(), Token::Identifier);
    structure->identifier = data.code.front().data;
    data.code.pop_front();

    if (data.code.front().data == "{")
    {
      data.code.pop_front();
      while (data.code.front().data != "}")
      {
        structure->members.emplace_back(Declaration::parse(data), -1);

        if (data.code.front().data == ":")
        {
          data.code.pop_front();

          ParseError::expect(data.code.front(), Token::Constant);
          structure->members.back().second = std::stoi(data.code.front().data);
          data.code.pop_front();
        }

        ParseError::expect(data.code.front().data, ";");
        data.code.pop_front();
      }
      data.code.pop_front();
    }

  } else if (dataType->generalType == DataType::GeneralType::Function)
  {
    Function* function = (Function*)dataType;

    std::list<Token> buffer;
    // we only use the section of the code with the return type so it doesn't recursively create functions
    while (data.code.front().data != "(")
    {
      buffer.push_back(data.code.front());
      data.code.pop_front();
    }
    // need to add a dummy token to the end to avoid segfault when checking first element of list
    buffer.push_back(Token{Token::Other, ";"});

    function->returnType = std::unique_ptr<DataType>(DataType::parse(data));

    buffer.pop_back();
    while (!buffer.empty())
    {
      data.code.push_front(buffer.back());
      buffer.pop_back();
    }

    if (data.code.front().data == "(")
    {
      data.code.pop_front();
    }
    
    while (data.code.front().data != "(")
    {
      // this effectively removes the closing parentheses for function pointers
      if (data.code.front().data != ")")
      {
        buffer.push_front(data.code.front());
      }

      data.code.pop_front();
    }
    data.code.pop_front();

    if (data.code.front().data == "void")
    {
      data.code.pop_front();
    }
    
    while (data.code.front().data != ")")
    {
      if (data.code.front().data == ",")
      {
        data.code.pop_front();
      }

      function->parameters.emplace_back(std::unique_ptr<Declaration>(Declaration::parse(data)));
    }
    data.code.pop_front();

    while (!buffer.empty())
    {
      data.code.push_front(buffer.front());
      buffer.pop_front();
    }
  } else if (data.code.front().data == "enum")
  {
    data.code.pop_front();

    ParseError::expect(data.code.front(), Token::Identifier);
    if (!data.program->enumTypes.contains(data.code.front().data))
    {
      std::cout << "Parse error: \"" << data.code.front().data << "\" is not a valid enum type\n";
      throw ParseError();
    }
    data.code.pop_front();

    *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.intSize};

  } else if (dataType->generalType == DataType::GeneralType::PrimitiveType)
  {
    PrimitiveType* primitiveType = (PrimitiveType*)dataType;
    
    switch (ParseError::expect(data.code.front().data, {"void", "signed", "unsigned", "short", "long", "char", "int", "float", "double"})) {
      case 0:
        primitiveType->size = 0;
        data.code.pop_front();
        break;
      case 1:
        data.code.pop_front();

        if (data.code.front().data == "short")
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.shortSize};
          data.code.pop_front();
          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        } else if (data.code.front().data == "long")
        {
          data.code.pop_front();
          if (data.code.front().data == "long")
          {
            *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.longLongSize};
            data.code.pop_front();
          } else
          {
            *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.longSize};
          }
          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        } else if (data.code.front().data == "char")
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.charSize};
          data.code.pop_front();
        } else
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.intSize};

          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        }
        break;
      case 2:
        data.code.pop_front();

        if (data.code.front().data == "short")
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, false, data.typeSizes.shortSize};
          data.code.pop_front();
          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        } else if (data.code.front().data == "long")
        {
          data.code.pop_front();
          if (data.code.front().data == "long")
          {
            *((PrimitiveType*)dataType) = PrimitiveType{false, false, data.typeSizes.longLongSize};
            data.code.pop_front();
          } else
          {
            *((PrimitiveType*)dataType) = PrimitiveType{false, false, data.typeSizes.longSize};
          }
          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        } else if (data.code.front().data == "char")
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, false, data.typeSizes.charSize};
          data.code.pop_front();
        } else
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, false, data.typeSizes.intSize};

          if (data.code.front().data == "int")
          {
            data.code.pop_front();
          }
        }
        break;
      case 3:
        *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.shortSize};
        data.code.pop_front();
        if (data.code.front().data == "int")
        {
          data.code.pop_front();
        }
        break;
      case 4:
        data.code.pop_front();
        if (data.code.front().data == "long")
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.longLongSize};
          data.code.pop_front();
        } else
        {
          *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.longSize};
        }
        if (data.code.front().data == "int")
        {
          data.code.pop_front();
        }
        break;
      case 5:
        *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.charSize};
        data.code.pop_front();
        break;
      case 6:
        *((PrimitiveType*)dataType) = PrimitiveType{false, true, data.typeSizes.intSize};
        data.code.pop_front();
        break;
      case 7:
        *((PrimitiveType*)dataType) = PrimitiveType{true, true, data.typeSizes.floatSize};
        data.code.pop_front();
        break;
      case 8:
        *((PrimitiveType*)dataType) = PrimitiveType{true, true, data.typeSizes.doubleSize};
        data.code.pop_front();
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

  while (data.code.front().data == "*")
  {
    Pointer* pointer = new Pointer;
    pointer->dataType = std::unique_ptr<DataType>(dataType);
    dataType = pointer;
    data.code.pop_front();
  }

  Token variableIdentifier;
  if ((data.code.front().type == Token::Identifier && (++data.code.begin())->data == "["))
  {
    variableIdentifier = data.code.front();
    data.code.pop_front();
  }

  while (data.code.front().data == "[")
  {
    Array* array = new Array;
    array->dataType = std::unique_ptr<DataType>(dataType);
    dataType = array;

    data.code.pop_front();
    array->size = std::unique_ptr<Expression>(Expression::parse(data));

    ParseError::expect(data.code.front().data, "]");
    data.code.pop_front();
  }

  if (variableIdentifier.type == Token::Identifier)
  {
    data.code.push_front(variableIdentifier);
  }

  return dataType;
}

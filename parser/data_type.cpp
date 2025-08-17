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

/*DataType* DataType::parse(CommonParseData& data)
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

    CommonParseData buffer;
    buffer.program = data.program;
    buffer.typeSizes = data.typeSizes;
    // we only use the section of the code with the return type so it doesn't recursively create functions
    while (data.code.front().data != "(")
    {
      buffer.code.push_back(data.code.front());
      data.code.pop_front();
    }
    // need to add a dummy token to the end to avoid segfault when checking first element of list
    buffer.code.push_back(Token{Token::Other, ";"});

    function->returnType = std::unique_ptr<DataType>(DataType::parse(buffer));

    buffer.code.pop_back();
    while (!buffer.code.empty())
    {
      data.code.push_front(buffer.code.back());
      buffer.code.pop_back();
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
        buffer.code.push_front(data.code.front());
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

    while (!buffer.code.empty())
    {
      data.code.push_front(buffer.code.front());
      buffer.code.pop_front();
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
}*/

DataType* DataType::parse(CommonParseData& data, std::list<Token>::iterator origin)
{
  DataType* dataType = nullptr;

  Token identifier;

  if (origin == std::list<Token>::iterator())
  {
    uint8_t depth = 0;
    bool enteredParentheses = false;
    for (origin = data.code.begin(); origin != data.code.end(); origin++)
    {
      if (origin->data == "{")
      {
        depth++;
      } else if (origin->data == "}")
      {
        depth--;
      } else if (enteredParentheses && ( // detecting if there is a function in the way
        origin->data == ")" ||
        origin->data == "enum" ||
        origin->data == "struct" ||
        origin->data == "union" ||
        origin->data == "void" ||
        origin->data == "signed" ||
        origin->data == "unsigned" ||
        origin->data == "short" ||
        origin->data == "short" ||
        origin->data == "long" ||
        origin->data == "char" ||
        origin->data == "int" ||
        origin->data == "float" ||
        origin->data == "double"))
      {
        origin--;
        origin--;
        break;
      } else if (
        depth == 0 &&
        (origin->data == "[" ||
        origin->data == ")" ||
        origin->data == ":" || 
        origin->data == ";" || 
        origin->data == "," ||
        origin->data == "="))
      {
        origin--;
        break;
      }

      enteredParentheses = (origin->data == "(");
    }

    std::list<Token>::iterator previous = origin;
    previous--;
    if (origin->type == Token::Identifier && previous->data != "struct" && previous->data != "union" && previous->data != "enum")
    {
      identifier = *origin;
      data.code.erase(origin--);
    }
  }

  std::list<Token>::iterator next = origin;
  next++;

  if (next->data == "[")
  {
    data.code.erase(next++);

    dataType = new Array;
    Array* array = (Array*)dataType;
    
    CommonParseData buffer;
    buffer.program = data.program;
    buffer.typeSizes = data.typeSizes;
    for (uint8_t depth = 1; depth > 0;)
    {
      if (next->data == "[")
      {
        depth++;
      } else if (next->data == "]")
      {
        depth--;
      }

      buffer.code.push_back(*next);
      data.code.erase(next++);
    }

    array->size = std::unique_ptr<Expression>(Expression::parse(buffer, true));

    array->dataType = std::unique_ptr<DataType>(DataType::parse(data, origin));
  } else if (next->data == "(")
  {
    data.code.erase(next++);

    dataType = new Function;
    Function* function = (Function*)dataType;
    
    CommonParseData buffer;
    buffer.program = data.program;
    buffer.typeSizes = data.typeSizes;
    for (uint8_t depth = 1; depth > 0;)
    {
      if (next->data == "(")
      {
        depth++;
      } else if (next->data == ")")
      {
        depth--;
      }

      buffer.code.push_back(*next);
      data.code.erase(next++);
    }

    while (buffer.code.size() > 1)
    {
      if (buffer.code.front().data == ",")
      {
        buffer.code.pop_front();
      }

      function->parameters.emplace_back(Declaration::parse(buffer));
    }

    function->returnType = std::unique_ptr<DataType>(DataType::parse(data, origin));
  } else
  {
    bool isVolatile = false;
    bool isConst = false;
    if (origin->data == "const")
    {
      isConst = true;

      data.code.erase(origin--);
    }

    if (origin->type == Token::Keyword && (
      origin->data == "void" ||
      origin->data == "signed" ||
      origin->data == "unsigned" ||
      origin->data == "short" ||
      origin->data == "long" ||
      origin->data == "char" ||
      origin->data == "int" ||
      origin->data == "float" ||
      origin->data == "double"))
    {
      dataType = new PrimitiveType;
      PrimitiveType* primitiveType = (PrimitiveType*)dataType;

      if (origin->data == "void")
      {
        data.code.erase(origin--);
        *((PrimitiveType*)dataType) = PrimitiveType{false, false, 0};
      } else if (origin->data == "float")
      {
        data.code.erase(origin--);
        *((PrimitiveType*)dataType) = PrimitiveType{true, true, data.typeSizes.floatSize};
      } else if (origin->data == "double")
      {
        data.code.erase(origin--);
        ((PrimitiveType*)dataType)->isFloating = true;
        ((PrimitiveType*)dataType)->isSigned = true;

        if (origin->data == "long")
        {
          data.code.erase(origin--);
          ((PrimitiveType*)dataType)->size = data.typeSizes.longDoubleSize;
        } else
        {
          ((PrimitiveType*)dataType)->size = data.typeSizes.doubleSize;
        }
      } else
      {
        if (origin->data == "int")
        {
          data.code.erase(origin--);
        }

        if (origin->data == "char")
        {
          data.code.erase(origin--);
          ((PrimitiveType*)dataType)->size = data.typeSizes.charSize;
        } else if (origin->data == "short")
        {
          data.code.erase(origin--);
          ((PrimitiveType*)dataType)->size = data.typeSizes.shortSize;
        } else if (origin->data == "long")
        {
          data.code.erase(origin--);
          if (origin->data == "long")
          {
            data.code.erase(origin--);
            ((PrimitiveType*)dataType)->size = data.typeSizes.longLongSize;
          } else
          {
            ((PrimitiveType*)dataType)->size = data.typeSizes.longSize;
          }
        } else
        {
          ((PrimitiveType*)dataType)->size = data.typeSizes.intSize;
        }
      }

      if (origin->data == "signed")
      {
        data.code.erase(origin--);
        ((PrimitiveType*)dataType)->isSigned = true;
      } else if (origin->data == "unsigned")
      {
        data.code.erase(origin--);
        ((PrimitiveType*)dataType)->isSigned = false;
      }

      if (origin->data == "volatile")
      {
        isVolatile = true;

        data.code.erase(origin--);
      }

      if (origin->data == "const")
      {
        isConst = true;

        data.code.erase(origin--);
      }
    } else if (origin->type == Token::Identifier)
    {
      if (data.program->typedefs.contains(origin->data))
      {
        dataType = data.program->typedefs[origin->data].get();
        data.code.erase(origin--);
      } else
      {
        std::string typeIdentifier = origin->data;
        
        data.code.erase(origin--);

        if (origin->data == "struct")
        {
          data.code.erase(origin--);

          dataType = new Struct;
          Struct* structure = (Struct*)dataType;

          structure->identifier = typeIdentifier;

        } else if (origin->data == "enum")
        {
          data.code.erase(origin--);

          dataType = new PrimitiveType;
          PrimitiveType* primitiveType = (PrimitiveType*)dataType;

          *primitiveType = PrimitiveType{false, true, data.typeSizes.intSize};
        }
      }
    } else if (origin->data == "}")
    {
      data.code.erase(origin--);

      std::vector<std::pair<Declaration*, uint8_t>> members;

      CommonParseData buffer;
      buffer.program = data.program;
      buffer.typeSizes = data.typeSizes;
      for (uint8_t depth = 1; depth > 0;)
      {
        if (origin->data == "{")
        {
          depth--;
          if (depth == 0)
          {
            data.code.erase(origin--);
            break;
          }
        } else if (origin->data == "}")
        {
          depth++;
        }

        buffer.code.push_front(*origin);
        data.code.erase(origin--);
      }

      while (buffer.code.size() > 1)
      {
        members.emplace_back(Declaration::parse(buffer), -1);

        if (buffer.code.front().data == ":")
        {
          buffer.code.pop_front();

          ParseError::expect(buffer.code.front(), Token::Constant);
          members.back().second = std::stoi(buffer.code.front().data);
          buffer.code.pop_front();
        }

        if (buffer.code.front().data == ";")
        {
          buffer.code.pop_front();
        }
      }

      dataType = parse(data, origin);
      if (dataType->generalType == GeneralType::Struct)
      {
        Struct* structure = (Struct*)dataType;

        for (std::pair<Declaration*, uint8_t> member: members)
        {
          structure->members.emplace_back(member.first, member.second);
        }
      }
    } else if (origin->data == "*")
    {
      data.code.erase(origin--);

      dataType = new Pointer;

      ((Pointer*)dataType)->dataType = std::unique_ptr<DataType>(parse(data, origin));
    } else if (origin->data == "(")
    {
      data.code.erase(next++);
      data.code.erase(origin--);

      dataType = parse(data, origin);
    }

    dataType->isVolatile = isVolatile;
    dataType->isConst = isConst;
  }

  if (identifier.type == Token::Identifier)
  {
    data.code.push_front(identifier);
  }

  return dataType;
}

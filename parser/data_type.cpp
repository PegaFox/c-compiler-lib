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
        origin->data == "const" ||
        origin->data == "extern" ||
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

    dataType = data.program->arenaAlloc((Array*)dataType);

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

    array->size = Expression::parse(buffer, true);

    array->dataType = DataType::parse(data, origin);
  } else if (next->data == "(")
  {
    data.code.erase(next++);

    dataType = data.program->arenaAlloc((Function*)dataType);

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

    function->returnType = DataType::parse(data, origin);
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
      dataType = data.program->arenaAlloc((PrimitiveType*)dataType);

      PrimitiveType* primitiveType = (PrimitiveType*)dataType;

      if (origin->data == "void")
      {
        data.code.erase(origin--);
        *((PrimitiveType*)dataType) = PrimitiveType{0, 0};
      } else if (origin->data == "float")
      {
        data.code.erase(origin--);
        *((PrimitiveType*)dataType) = PrimitiveType{data.typeSizes.floatSize, data.typeSizes.floatSize, true, true};
      } else if (origin->data == "double")
      {
        data.code.erase(origin--);
        ((PrimitiveType*)dataType)->isFloating = true;
        ((PrimitiveType*)dataType)->isSigned = true;

        if (origin->data == "long")
        {
          data.code.erase(origin--);
          ((PrimitiveType*)dataType)->size = data.typeSizes.longDoubleSize;
          ((PrimitiveType*)dataType)->alignment = data.typeSizes.longDoubleSize;
        } else
        {
          ((PrimitiveType*)dataType)->size = data.typeSizes.doubleSize;
          ((PrimitiveType*)dataType)->alignment = data.typeSizes.doubleSize;
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
          ((PrimitiveType*)dataType)->alignment = data.typeSizes.charSize;
        } else if (origin->data == "short")
        {
          data.code.erase(origin--);
          ((PrimitiveType*)dataType)->size = data.typeSizes.shortSize;
          ((PrimitiveType*)dataType)->alignment = data.typeSizes.shortSize;
        } else if (origin->data == "long")
        {
          data.code.erase(origin--);
          if (origin->data == "long")
          {
            data.code.erase(origin--);
            ((PrimitiveType*)dataType)->size = data.typeSizes.longLongSize;
            ((PrimitiveType*)dataType)->alignment = data.typeSizes.longLongSize;
          } else
          {
            ((PrimitiveType*)dataType)->size = data.typeSizes.longSize;
            ((PrimitiveType*)dataType)->alignment = data.typeSizes.longSize;
          }
        } else
        {
          ((PrimitiveType*)dataType)->size = data.typeSizes.intSize;
          ((PrimitiveType*)dataType)->alignment = data.typeSizes.intSize;
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
        dataType = data.program->typedefs[origin->data];
        data.code.erase(origin--);
      } else
      {
        std::string typeIdentifier = origin->data;
        
        data.code.erase(origin--);

        if (origin->data == "struct")
        {
          data.code.erase(origin--);

          dataType = data.program->arenaAlloc((Struct*)dataType);

          Struct* structure = (Struct*)dataType;

          structure->identifier = data.program->arenaAlloc(typeIdentifier);

        } else if (origin->data == "enum")
        {
          data.code.erase(origin--);

          dataType = data.program->arenaAlloc((PrimitiveType*)dataType);

          PrimitiveType* primitiveType = (PrimitiveType*)dataType;

          *primitiveType = PrimitiveType{data.typeSizes.intSize, data.typeSizes.intSize, false, true};
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

      dataType = data.program->arenaAlloc((Pointer*)dataType);

      ((Pointer*)dataType)->dataType = parse(data, origin);
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

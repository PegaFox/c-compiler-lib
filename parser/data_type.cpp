#include "data_type.hpp"

#include "parse_error.hpp"
#include "primitive_type.hpp"
#include "pointer.hpp"
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
        structure->members.emplace_back(VariableDeclaration::parse(code, program), -1);

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
        switch (ParseError::expect(code.front().data, {"short", "long", "char", "int"})) {
          case 0:
            primitiveType->type = PrimitiveType::Type::SignedShort;
            code.pop_front();
            if (code.front().data == "int")
            {
              code.pop_front();
            }
            break;
          case 1:
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
          case 2:
            primitiveType->type = PrimitiveType::Type::SignedChar;
            code.pop_front();
            break;
          case 3:
            primitiveType->type = PrimitiveType::Type::SignedInt;
            code.pop_front();
            break;
          default:

            break;
        }
        break;
      case 2:
        code.pop_front();
        switch (ParseError::expect(code.front().data, {"short", "long", "char", "int"}))
        {
          case 0:
            primitiveType->type = PrimitiveType::Type::UnsignedShort;
            code.pop_front();
            if (code.front().data == "int")
            {
              code.pop_front();
            }
            break;
          case 1:
            code.pop_front();
            if (code.front().data == "long")
            {
              primitiveType->type = PrimitiveType::Type::UnsignedLongLong;
              code.pop_front();
            } else {
              primitiveType->type = PrimitiveType::Type::UnsignedLong;
            }
            if (code.front().data == "int")
            {
              code.pop_front();
            }
            break;
          case 2:
            primitiveType->type = PrimitiveType::Type::UnsignedChar;
            code.pop_front();
            break;
          case 3:
            primitiveType->type = PrimitiveType::Type::UnsignedInt;
            code.pop_front();
            break;
          default:

            break;
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

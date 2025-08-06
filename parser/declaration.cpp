#include "declaration.hpp"
#include <iostream>

#include "expression.hpp"
#include "compound_statement.hpp"

Declaration::Declaration()
{
  statementType = StatementType::Declaration;
}

Declaration* Declaration::parse(CommonParseData& data, Linkage defaultLinkage)
{
  Declaration* declaration = new Declaration;

  std::vector<Token> buffer;
  bool namedType = false;
  for (std::list<Token>::iterator type = data.code.begin(); type->type == Token::Keyword || namedType; type++)
  {
    if (type->data == "enum" || type->data == "struct" || type->data == "union")
    {
      namedType = true;
    } else
    {
      namedType = false;
    }
    buffer.push_back(*type);
  }

  declaration->linkage = defaultLinkage;
  bool changed = true;
  while (changed)
  {
    changed = false;

    if (data.code.front().data == "inline")
    {
      changed = true;
      declaration->isInline = true;
      data.code.pop_front();
    }

    if (data.code.front().data == "typedef")
    {
      changed = true;
      declaration->isTypedef = true;
      data.code.pop_front();
    }

    if (data.code.front().data == "static")
    {
      changed = true;
      declaration->linkage = Linkage::Internal;
      data.code.pop_front();
    } else if (data.code.front().data == "extern")
    {
      changed = true;
      declaration->linkage = Linkage::External;
      data.code.pop_front();
    }
  }

  declaration->dataType = std::unique_ptr<DataType>(DataType::parse(data));

  if (data.code.front().type == Token::Identifier)
  {
    declaration->identifier = data.code.front().data;
    data.code.pop_front();

    if (data.code.front().data == "=")
    {
      data.code.pop_front();
      declaration->value = std::unique_ptr<Statement>(Expression::parse(data, false));
    } else if (data.code.front().data == "{")
    {
      declaration->value = std::unique_ptr<Statement>(CompoundStatement::parse(data));
    } else if (declaration->isTypedef)
    {
      data.program->typedefs[declaration->identifier] = std::unique_ptr<DataType>(declaration->dataType.get());
    }

    // Need to check if code is empty to avoid a segfault after the final function
    if (data.code.size() > 1 && data.code.front().data == "," && (++data.code.begin())->type != Token::Keyword && !data.program->typedefs.contains((++data.code.begin())->data))
    {
      data.code.pop_front();

      while (!buffer.empty())
      {
        data.code.push_front(buffer.back());
        buffer.pop_back();
      }
      data.code.push_front(Token{Token::Other, ";"});
    }
    //ParseError::expect(code.front().data, {";", ",", ")"});
    //code.pop_front();
  }

  return declaration;
}

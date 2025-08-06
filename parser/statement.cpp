#include "statement.hpp"

#include <iostream>

#include "parse_error.hpp"
#include "return.hpp"
#include "break.hpp"
#include "continue.hpp"
#include "goto.hpp"
#include "if_conditional.hpp"
#include "switch_case.hpp"
#include "switch_default.hpp"
#include "switch_conditional.hpp"
#include "do_while_loop.hpp"
#include "while_loop.hpp"
#include "for_loop.hpp"
#include "declaration.hpp"
#include "compound_statement.hpp"
#include "label.hpp"

Statement::Statement()
{
  nodeType = NodeType::Statement;
}

Statement* Statement::parse(CommonParseData& data, bool canParseVariableDeclarations)
{
  Statement* statement = nullptr;

  if (data.code.front().type == Token::Keyword || data.program->typedefs.contains(data.code.front().data))
  {
    if (data.code.front().data == "return")
    {
      statement = Return::parse(data);
    } else if (data.code.front().data == "break")
    {
      statement = Break::parse(data);
    } else if (data.code.front().data == "continue")
    {
      statement = Continue::parse(data);
    } else if (data.code.front().data == "goto")
    {
      statement = Goto::parse(data);
    } else if (data.code.front().data == "if")
    {
      statement = IfConditional::parse(data);
    } else if (data.code.front().data == "case")
    {
      statement = SwitchCase::parse(data);
    } else if (data.code.front().data == "default")
    {
      statement = SwitchDefault::parse(data);
    } else if (data.code.front().data == "switch")
    {
      statement = SwitchConditional::parse(data);
    } else if (data.code.front().data == "do")
    {
      statement = DoWhileLoop::parse(data);
    } else if (data.code.front().data == "while")
    {
      statement = WhileLoop::parse(data);
    } else if (data.code.front().data == "for")
    {
      statement = ForLoop::parse(data);
    } else if (
      data.code.front().data == "signed" ||
      data.code.front().data == "unsigned" ||
      data.code.front().data == "static" ||
      data.code.front().data == "extern" ||
      data.code.front().data == "const" ||
      data.code.front().data == "char" ||
      data.code.front().data == "short" ||
      data.code.front().data == "int" ||
      data.code.front().data == "long" ||
      data.code.front().data == "float" ||
      data.code.front().data == "double" ||
      data.code.front().data == "struct" || 
      data.program->typedefs.contains(data.code.front().data))
    {
      if (canParseVariableDeclarations)
      {
        statement = Declaration::parse(data);
        ParseError::expect(data.code.front().data, ";");
        data.code.pop_front();
      }
    }
  } else if (data.code.front().type == Token::Identifier && (++data.code.begin())->data == ":")
  {
    statement = Label::parse(data);
  } else if (data.code.front().data == "{")
  {
    statement = CompoundStatement::parse(data);
  } else
  {
    statement = Expression::parse(data);

    ParseError::expect(data.code.front().data, ";");
    data.code.pop_front();
  }

  if (statement == nullptr)
  {
    std::cout << "Parse error: Expected a statement\n";
    throw ParseError();
  }

  return statement;
}

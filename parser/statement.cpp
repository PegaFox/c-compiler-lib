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

Statement* Statement::parse(std::list<Token>& code, Program& program, bool canParseVariableDeclarations)
{
  Statement* statement = nullptr;

  if (code.front().type == Token::Keyword)
  {
    if (code.front().data == "return")
    {
      statement = Return::parse(code, program);
    } else if (code.front().data == "break")
    {
      statement = Break::parse(code);
    } else if (code.front().data == "continue")
    {
      statement = Continue::parse(code);
    } else if (code.front().data == "goto")
    {
      statement = Goto::parse(code);
    } else if (code.front().data == "if")
    {
      statement = IfConditional::parse(code, program);
    } else if (code.front().data == "case")
    {
      statement = SwitchCase::parse(code, program);
    } else if (code.front().data == "default")
    {
      statement = SwitchDefault::parse(code);
    } else if (code.front().data == "switch")
    {
      statement = SwitchConditional::parse(code, program);
    } else if (code.front().data == "do")
    {
      statement = DoWhileLoop::parse(code, program);
    } else if (code.front().data == "while")
    {
      statement = WhileLoop::parse(code, program);
    } else if (code.front().data == "for")
    {
      statement = ForLoop::parse(code, program);
    } else if (
      code.front().data == "signed" ||
      code.front().data == "unsigned" ||
      code.front().data == "static" ||
      code.front().data == "extern" ||
      code.front().data == "const" ||
      code.front().data == "char" ||
      code.front().data == "short" ||
      code.front().data == "int" ||
      code.front().data == "long" ||
      code.front().data == "float" ||
      code.front().data == "double" ||
      code.front().data == "struct" || 
      program.typedefs.contains(code.front().data))
    {
      for (const Token& token: code)
      {
        if (token.data == "=" || token.data == ";")
        {
          if (canParseVariableDeclarations)
          {
            statement = Declaration::parse(code, program);
            ParseError::expect(code.front().data, ";");
            code.pop_front();
          }
          break;
        }
      }
    }
  } else if (code.front().type == Token::Identifier && (++code.begin())->data == ":")
  {
    statement = Label::parse(code);
  } else if (code.front().data == "{")
  {
    statement = CompoundStatement::parse(code, program);
  } else
  {
    statement = Expression::parse(code, program);

    ParseError::expect(code.front().data, ";");
    code.pop_front();
  }

  if (statement == nullptr)
  {
    std::cout << "Parse error: Expected a statement\n";
    throw ParseError();
  }

  return statement;
}

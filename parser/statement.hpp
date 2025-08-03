#ifndef PF_PARSER_STATEMENT_HPP
#define PF_PARSER_STATEMENT_HPP

#include "program.hpp"

struct Statement: public ASTnode
{
  enum class StatementType
  {
    Undefined, // if a node has this type, something went wrong
    CompoundStatement,
    Expression,
    Label,
    Return,
    Break,
    Continue,
    Goto,
    SwitchCase,
    SwitchDefault,
    Declaration,
    IfConditional,
    SwitchConditional,
    DoWhileLoop,
    WhileLoop,
    ForLoop
  } statementType;

  Statement();

  static Statement* parse(CommonParseData& data, bool canParseVariableDeclarations = false);
};

#endif // PF_PARSER_STATEMENT_HPP

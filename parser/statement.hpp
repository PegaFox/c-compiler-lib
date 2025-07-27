#ifndef PF_PARSER_STATEMENT_HPP
#define PF_PARSER_STATEMENT_HPP

#include "AST_node.hpp"

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
    VariableDeclaration,
    FunctionDeclaration,
    IfConditional,
    SwitchConditional,
    DoWhileLoop,
    WhileLoop,
    ForLoop
  } statementType;

  Statement();
};

#endif // PF_PARSER_STATEMENT_HPP

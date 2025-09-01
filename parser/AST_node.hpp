#ifndef PF_PARSER_AST_NODE_HPP
#define PF_PARSER_AST_NODE_HPP

#include <memory>
#include "../compiler.hpp"
#include "../lexer.hpp"

class Program;

struct ASTnode
{
  enum class NodeType
  {
    Undefined, // if a node has this type, something went wrong
    Statement,
    Program,
    DataType
  } nodeType;

  protected:
    struct CommonParseData
    {
      std::list<Token> code;
      Program* program;
      Compiler::TypeSizes typeSizes;
    };
};

#endif // PF_PARSER_AST_NODE_HPP

#ifndef PF_PARSER_AST_NODE_HPP
#define PF_PARSER_AST_NODE_HPP

struct ASTnode
{
  enum class NodeType
  {
    Undefined, // if a node has this type, something went wrong
    Statement,
    Program,
    DataType
  } nodeType;
};

#endif // PF_PARSER_AST_NODE_HPP
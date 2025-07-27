#ifndef PF_PARSER_DATA_TYPE_HPP
#define PF_PARSER_DATA_TYPE_HPP

#include "AST_node.hpp"

struct DataType: public ASTnode
{
  enum class GeneralType
  {
    Undefined, // if a node has this type, something went wrong
    PrimitiveType,
    Pointer,
    Array,
    Struct
  } generalType;

  enum class Linkage
  {
    None,
    Internal,
    External
  } linkage;

  bool isConst = false;
  bool isVolatile = false;

  DataType();
};

#endif // PF_PARSER_DATA_TYPE_HPP

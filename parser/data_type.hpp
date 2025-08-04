#ifndef PF_PARSER_DATA_TYPE_HPP
#define PF_PARSER_DATA_TYPE_HPP

#include "program.hpp"

struct DataType: public ASTnode
{
  enum class GeneralType
  {
    Undefined, // if a node has this type, something went wrong
    PrimitiveType,
    Pointer,
    Function,
    Array,
    Struct
  } generalType;

  
  bool isConst = false;
  bool isVolatile = false;

  DataType();

  //static DataType* parse(CommonParseData& data);
  static DataType* parse(CommonParseData& data, std::list<Token>::iterator origin = std::list<Token>::iterator());
};

#endif // PF_PARSER_DATA_TYPE_HPP

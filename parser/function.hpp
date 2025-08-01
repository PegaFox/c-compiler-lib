#ifndef PF_PARSER_FUNCTION_HPP
#define PF_PARSER_FUNCTION_HPP

#include "declaration.hpp"

struct Function: public DataType
{
  std::unique_ptr<DataType> returnType;

  std::vector<std::unique_ptr<Declaration>> parameters;

  Function();

  //static Function* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_FUNCTION_HPP

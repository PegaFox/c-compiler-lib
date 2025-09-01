#ifndef PF_PARSER_FUNCTION_HPP
#define PF_PARSER_FUNCTION_HPP

#include "declaration.hpp"

struct Function: public DataType
{
  DataType* returnType = nullptr;

  std::vector<Declaration*> parameters;

  Function();

  //static Function* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_FUNCTION_HPP

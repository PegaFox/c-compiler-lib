#ifndef PF_PARSER_FUNCTION_CALL_HPP
#define PF_PARSER_FUNCTION_CALL_HPP

#include <vector>
#include <memory>

#include "expression.hpp"

struct FunctionCall: public Expression
{
  std::string identifier;

  std::vector<std::unique_ptr<Expression>> arguments;

  FunctionCall();

  static FunctionCall* parse(std::list<Token>& code);
};

#endif // PF_PARSER_FUNCTION_CALL_HPP

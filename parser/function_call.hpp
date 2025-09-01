#ifndef PF_PARSER_FUNCTION_CALL_HPP
#define PF_PARSER_FUNCTION_CALL_HPP

#include <vector>
#include <memory>

#include "expression.hpp"

struct FunctionCall: public Expression
{
  std::string_view identifier;

  std::vector<Expression*> arguments;

  FunctionCall();

  static FunctionCall* parse(CommonParseData& data);
};

#endif // PF_PARSER_FUNCTION_CALL_HPP

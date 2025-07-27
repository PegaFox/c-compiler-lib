#ifndef PF_PARSER_VARIABLE_ACCESS_HPP
#define PF_PARSER_VARIABLE_ACCESS_HPP

#include <string>

#include "expression.hpp"

struct VariableAccess: public Expression
{
  std::string identifier;

  VariableAccess();

  static VariableAccess* parse(std::list<Token>& code);
};

#endif // PF_PARSER_VARIABLE_ACCESS_HPP

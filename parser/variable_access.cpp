#include "variable_access.hpp"

VariableAccess::VariableAccess()
{
  expressionType = ExpressionType::VariableAccess;
}

VariableAccess* VariableAccess::parse(std::list<Token>& code)
{
  VariableAccess* variableAccess = new VariableAccess;

  variableAccess->identifier = code.front().data;
  code.pop_front();

  return variableAccess;
}

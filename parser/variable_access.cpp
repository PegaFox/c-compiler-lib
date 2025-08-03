#include "variable_access.hpp"

VariableAccess::VariableAccess()
{
  expressionType = ExpressionType::VariableAccess;
}

VariableAccess* VariableAccess::parse(CommonParseData& data)
{
  VariableAccess* variableAccess = new VariableAccess;

  variableAccess->identifier = data.code.front().data;
  data.code.pop_front();

  return variableAccess;
}

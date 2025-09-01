#include "variable_access.hpp"

VariableAccess::VariableAccess()
{
  expressionType = ExpressionType::VariableAccess;
}

VariableAccess* VariableAccess::parse(CommonParseData& data)
{
  VariableAccess* variableAccess;
  variableAccess = data.program->arenaAlloc(variableAccess);

  variableAccess->identifier = data.program->arenaAlloc(data.code.front().data);
  data.code.pop_front();

  return variableAccess;
}

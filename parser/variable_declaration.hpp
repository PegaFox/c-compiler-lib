#ifndef PF_PARSER_VARIABLE_DECLARATION_HPP
#define PF_PARSER_VARIABLE_DECLARATION_HPP

#include <memory>

#include "data_type.hpp"
#include "expression.hpp"

struct VariableDeclaration: public Statement
{
  std::unique_ptr<DataType> dataType;

  std::string identifier;

  std::unique_ptr<Expression> value;

  VariableDeclaration();

  static VariableDeclaration* parse(std::list<Token>& code);
};

#endif // PF_PARSER_VARIABLE_DECLARATION_HPP

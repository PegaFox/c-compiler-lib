#ifndef PF_PARSER_DECLARATION_HPP
#define PF_PARSER_DECLARATION_HPP

#include <memory>

#include "data_type.hpp"
#include "expression.hpp"

struct Declaration: public Statement
{
  bool isTypedef = false;

  std::unique_ptr<DataType> dataType;

  std::string identifier;

  std::unique_ptr<Statement> value;

  Declaration();

  static Declaration* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_DECLARATION_HPP

#ifndef PF_PARSER_RETURN_HPP
#define PF_PARSER_RETURN_HPP

#include <memory>

#include "expression.hpp"

struct Return: public Statement
{
  Expression* data = nullptr;

  Return(); 

  static Return* parse(CommonParseData& data);
};

#endif // PF_PARSER_RETURN_HPP

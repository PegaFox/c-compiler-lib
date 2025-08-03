#ifndef PF_PARSER_RETURN_HPP
#define PF_PARSER_RETURN_HPP

#include <memory>

#include "expression.hpp"

struct Return: public Statement
{
  std::unique_ptr<Expression> data;

  Return(); 

  static Return* parse(CommonParseData& data);
};

#endif // PF_PARSER_RETURN_HPP

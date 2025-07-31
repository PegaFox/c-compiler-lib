#ifndef PF_PARSER_RETURN_HPP
#define PF_PARSER_RETURN_HPP

#include <memory>

#include "expression.hpp"

struct Return: public Statement
{
  std::unique_ptr<Expression> data;

  Return(); 

  static Return* parse(std::list<Token>& code, Program& program);
};

#endif // PF_PARSER_RETURN_HPP

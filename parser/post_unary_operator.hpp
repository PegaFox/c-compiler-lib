#ifndef PF_PARSER_POST_UNAR_OOPERATOR_HPP
#define PF_PARSER_POST_UNAR_OOPERATOR_HPP

#include <memory>

#include "expression.hpp"

struct PostUnaryOperator: public Expression
{
  enum class PostUnaryType
  {
    Undefined, // if a node has this type, something went wrong
    Increment,
    Decrement
  } postUnaryType;

  std::unique_ptr<Expression> operand;

  PostUnaryOperator();
};

#endif // PF_PARSER_POST_UNAR_OOPERATOR_HPP

#ifndef PF_PARSER_PRE_UNAR_OOPERATOR_HPP
#define PF_PARSER_PRE_UNAR_OOPERATOR_HPP

#include <memory>

#include "expression.hpp"

struct PreUnaryOperator: public Expression
{
  enum class PreUnaryType
  {
    Undefined, // if a node has this type, something went wrong
    MathematicNegate,
    BitwiseNOT,
    LogicalNegate,
    Increment,
    Decrement,
    Dereference,
    TypeCast,
    Address
  } preUnaryType;

  std::unique_ptr<Expression> operand;

  PreUnaryOperator();
};

#endif // PF_PARSER_PRE_UNAR_OOPERATOR_HPP

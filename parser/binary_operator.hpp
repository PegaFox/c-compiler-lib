#ifndef PF_PARSER_BINARY_OPERATOR_HPP
#define PF_PARSER_BINARY_OPERATOR_HPP

#include <memory>

#include "expression.hpp"

struct BinaryOperator: public Expression
{
  enum class BinaryType
  {
    Undefined, // if a node has this type, something went wrong
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    LeftShift,
    RightShift,
    BitwiseOR,
    BitwiseAND,
    BitwiseXOR,
    LogicalOR,
    LogicalAND,
    Subscript,
    VariableAssignment,
    AddEqual,
    SubtractEqual,
    MultiplyEqual,
    DivideEqual,
    ModuloEqual,
    LeftShiftEqual,
    RightShiftEqual,
    BitwiseOREqual,
    BitwiseANDEqual,
    BitwiseXOREqual,
    Equal,
    NotEqual,
    Greater,
    Lesser,
    GreaterOrEqual,
    LesserOrEqual
  } binaryType;

  static const constexpr uint8_t precedence[(uint8_t)BinaryType::LesserOrEqual+1] = {
    0, // Undefined
    11, // Add
    11, // Subtract
    12, // Multiply
    12, // Divide
    12, // Modulo
    10, // LeftShift
    10, // RightShift
    5, // BitwiseOR
    7, // BitwiseAND
    6, // BitwiseXOR
    3, // LogicalOR
    4, // LogicalAND
    13, // Subscript
    2, // VariableAssignment
    2, // AddEqual
    2, // SubtractEqual
    2, // MultiplyEqual
    2, // DivideEqual
    2, // ModuloEqual
    2, // LeftShiftEqual
    2, // RightShiftEqual
    2, // BitwiseOREqual
    2, // BitwiseANDEqual
    2, // BitwiseXOREqual
    8, // Equal
    8, // NotEqual
    9, // Greater
    9, // Lesser
    9, // GreaterOrEqual
    9 // LesserOrEqual
  };

  std::unique_ptr<Expression> leftOperand;

  std::unique_ptr<Expression> rightOperand;

  BinaryOperator();
};

#endif // PF_PARSER_BINARY_OPERATOR_HPP

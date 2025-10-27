#ifndef PF_PARSER_EXPRESSION_HPP
#define PF_PARSER_EXPRESSION_HPP

#include "statement.hpp"
#include "expression-parser/expression.hpp"

namespace OperatorType
{
  enum T
  {
    Null,
    Constant,
    FunctionCall,
    VariableAccess,
    StringLiteral,
    SubExpression,

    PreUnaryOperator,
    MathematicNegate,
    BitwiseNOT,
    LogicalNegate,
    PreIncrement,
    PreDecrement,
    Dereference,
    TypeCast,
    Address,
    Sizeof,

    PostUnaryOperator,
    PostIncrement,
    PostDecrement,

    BinaryOperator,
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
    MemberAccess,
    DereferenceMemberAccess,
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
    LesserOrEqual,

    TernaryOperator,
  };
}

struct Expression: public Statement
{
  enum class ExpressionType
  {
    Undefined, // if a node has this type, something went wrong
    Null,
    Constant,
    FunctionCall,
    VariableAccess,
    StringLiteral,
    SubExpression,
    PreUnaryOperator,
    PostUnaryOperator,
    BinaryOperator,
    TernaryOperator
  } expressionType;

  Expression();

  static Expression* parse(CommonParseData& data, bool allowNullExpression = true);
};

#endif // PF_PARSER_EXPRESSION_HPP

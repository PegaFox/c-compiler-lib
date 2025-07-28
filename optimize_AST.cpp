#include "optimize_AST.hpp"

#include "parser/AST_iterator.hpp"
#include "parser/pre_unary_operator.hpp"
#include "parser/binary_operator.hpp"
#include "parser/constant.hpp"
#include "parser/sub_expression.hpp"

void optimizeAST(Program& AST)
{
  bool changed;

  do
  {
    changed = false;
    changed |= optimizeConstants(AST);
  } while (changed);
}

bool optimizeConstants(Program& AST)
{
  bool changed = false;

  for (ASTiterator node(&AST); node != ASTiterator(nullptr); node++)
  {
    if (
      node->nodeType == ASTnode::NodeType::Statement &&
      ((Statement*)(*node))->statementType == Statement::StatementType::Expression)
    {
      if (((Expression*)((Statement*)(*node)))->expressionType == Expression::ExpressionType::BinaryOperator)
      {
        BinaryOperator* binaryOperator = (BinaryOperator*)(*node);

        if (
          binaryOperator->leftOperand.get()->expressionType == Expression::ExpressionType::Constant && 
          binaryOperator->rightOperand.get()->expressionType == Expression::ExpressionType::Constant)
        {
          Constant* leftConstant = (Constant*)(binaryOperator->leftOperand.get());
          Constant* rightConstant = (Constant*)(binaryOperator->rightOperand.get());

          switch (binaryOperator->binaryType)
          {
            case BinaryOperator::BinaryType::Add:
              leftConstant->value.unsignedChar += rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Subtract:
              leftConstant->value.unsignedChar -= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Multiply:
              leftConstant->value.unsignedChar *= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Divide:
              leftConstant->value.unsignedChar /= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Modulo:
              leftConstant->value.unsignedChar %= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::LeftShift:
              leftConstant->value.unsignedChar <<= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::RightShift:
              leftConstant->value.unsignedChar >>= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::BitwiseOR:
              leftConstant->value.unsignedChar |= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::BitwiseAND:
              leftConstant->value.unsignedChar &= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::BitwiseXOR:
              leftConstant->value.unsignedChar ^= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::LogicalOR:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar || rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::LogicalAND:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar && rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Greater:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar > rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Lesser:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar < rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::GreaterOrEqual:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar >= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::LesserOrEqual:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar <= rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::Equal:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar == rightConstant->value.unsignedChar;
              break;
            case BinaryOperator::BinaryType::NotEqual:
              leftConstant->value.unsignedChar = leftConstant->value.unsignedChar != rightConstant->value.unsignedChar;
              break;
            default:
              continue;
          }

          node.ptr = node.path.back().first;
          changed = true;

          binaryOperator->leftOperand.release();

          ((std::unique_ptr<Expression>*)node.path.back().second)->release();

          delete binaryOperator;
          *((std::unique_ptr<Expression>*)node.path.back().second) = std::unique_ptr<Expression>(leftConstant);
        }
      } else if (((Expression*)((Statement*)(*node)))->expressionType == Expression::ExpressionType::PreUnaryOperator)
      {
        PreUnaryOperator* preUnaryOperator = (PreUnaryOperator*)(*node);
        if (
          preUnaryOperator->operand.get()->expressionType == Expression::ExpressionType::Constant)
        {
          Constant* constant = (Constant*)(preUnaryOperator->operand.get());

          switch (preUnaryOperator->preUnaryType)
          {
            case PreUnaryOperator::PreUnaryType::MathematicNegate:
              constant->value.unsignedChar = -constant->value.unsignedChar;
              break;
            case PreUnaryOperator::PreUnaryType::BitwiseNOT:
              constant->value.unsignedChar = ~constant->value.unsignedChar;
              break;
            case PreUnaryOperator::PreUnaryType::LogicalNegate:
              constant->value.unsignedChar = !constant->value.unsignedChar;
              break;
            default:
              continue;
          }

          node.ptr = node.path.back().first;
          changed = true;

          preUnaryOperator->operand.release();

          ((std::unique_ptr<Expression>*)node.path.back().second)->release();

          delete preUnaryOperator;
          *((std::unique_ptr<Expression>*)node.path.back().second) = std::unique_ptr<Expression>(constant);
        }
      } else if (((Expression*)((Statement*)(*node)))->expressionType == Expression::ExpressionType::SubExpression)
      {
        SubExpression* subExpression = (SubExpression*)(*node);
        if (
          subExpression->expression.get()->expressionType == Expression::ExpressionType::Constant)
        {
          node.ptr = node.path.back().first;
          changed = true;

          Constant* constant = (Constant*)(subExpression->expression.get());

          subExpression->expression.release();

          ((std::unique_ptr<Expression>*)node.path.back().second)->release();

          delete subExpression;
          *((std::unique_ptr<Expression>*)node.path.back().second) = std::unique_ptr<Expression>(constant);
        }
      }
    }
  }

  return changed;
}

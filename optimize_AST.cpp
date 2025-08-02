#include "optimize_AST.hpp"

#include "parser/AST_iterator.hpp"
#include "parser/sub_expression.hpp"

void optimize_AST(Program& AST)
{
  bool changed;
  do
  {
    changed = false;
    uint32_t i = 0;
    for (ASTiterator node(&AST); node != ASTiterator(nullptr); node++)
    {
      if (
        node->nodeType == ASTnode::NodeType::Statement &&
        ((Statement*)(*node))->statementType == Statement::StatementType::Expression &&
        ((Expression*)(*node))->expressionType == Expression::ExpressionType::SubExpression)
      {
        SubExpression* subExpression = (SubExpression*)(*node);

        node.ptr = node.path.back().first;
        changed = true;

        Expression* expression = subExpression->expression.get();

        subExpression->expression.release();

        ((std::unique_ptr<Expression>*)node.path.back().second)->release();

        delete subExpression;
        *((std::unique_ptr<Expression>*)node.path.back().second) = std::unique_ptr<Expression>(expression);
      }
      i++;
    }
  } while (changed);
}

#include "AST_iterator.hpp"

#include "statement.hpp"
#include "compound_statement.hpp"
#include "expression.hpp"
#include "constant.hpp"
#include "function_call.hpp"
#include "sub_expression.hpp"
#include "pre_unary_operator.hpp"
#include "post_unary_operator.hpp"
#include "binary_operator.hpp"
#include "ternary_operator.hpp"
#include "return.hpp"
#include "declaration.hpp"
#include "function.hpp"
#include "if_conditional.hpp"
#include "switch_case.hpp"
#include "switch_conditional.hpp"
#include "do_while_loop.hpp"
#include "while_loop.hpp"
#include "for_loop.hpp"
#include "pointer.hpp"
#include "array.hpp"
#include "struct.hpp"

ASTiterator::ASTiterator(pointer ptr) : ptr(ptr)
{
  
}

ASTiterator::pointer ASTiterator::operator*() const {return ptr;}

ASTiterator::pointer ASTiterator::operator->() {return ptr;}

std::size_t ASTiterator::depth() {return path.size();}

ASTiterator& ASTiterator::operator++()
{
  bool first = true;

  topOfConditionals:
  if (!first && path.empty())
  {
    ptr = nullptr;
  }

  if (ptr == nullptr)
  {
    return *this;
  }
  
  first = false;
  switch (ptr->nodeType)
  {
    case ASTnode::NodeType::Statement: {
      Statement* statement = (Statement*)ptr;
      switch (statement->statementType)
      {
        case Statement::StatementType::CompoundStatement: {
          CompoundStatement* compoundStatement = (CompoundStatement*)statement;

          if (firstTime(compoundStatement))
          {
            if (compoundStatement->body.empty())
            {
              ptr = path.back().first;
              goto topOfConditionals;
            } else
            {
              path.push_back({compoundStatement, &(compoundStatement->body[0])});
              ptr = compoundStatement->body[0].get();
              break;
            }
          }

          for (std::vector<std::unique_ptr<Statement>>::iterator node = compoundStatement->body.begin(); node != compoundStatement->body.end(); node++)
          {
            if (&(*node) == path.back().second)
            {
              if (node+1 == compoundStatement->body.end())
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              } else
              {
                path.back().second = &(*(node+1));
                ptr = (node+1)->get();
              }
              break;
            }
          }
          break;
        } case Statement::StatementType::Expression: {
          Expression* expression = (Expression*)statement;
          switch (expression->expressionType)
          {
            case Expression::ExpressionType::Null:
              ptr = path.back().first;
              goto topOfConditionals;
              break;
            case Expression::ExpressionType::Constant: {
              Constant* constant = (Constant*)expression;
              if (firstTime(constant))
              {
                path.push_back({constant, &constant->dataType});
                ptr = constant->dataType.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::FunctionCall: {
              FunctionCall* functionCall = (FunctionCall*)statement;

              if (firstTime(functionCall))
              {
                if (functionCall->arguments.empty())
                {
                 ptr = path.back().first;
                 goto topOfConditionals;
               } else
               {
                  path.push_back({functionCall, &(functionCall->arguments[0])});
                  ptr = functionCall->arguments[0].get();
                  break;
                }
              }

              for (std::vector<std::unique_ptr<Expression>>::iterator node = functionCall->arguments.begin(); node != functionCall->arguments.end(); node++)
              {
                if (&(*node) == path.back().second)
                {
                  if (node+1 == functionCall->arguments.end())
                  {
                    path.pop_back();
                    ptr = path.back().first;
                    goto topOfConditionals;
                  } else
                  {
                    path.back().second = &(*(node+1));
                    ptr = (node+1)->get();
                  }
                  break;
                }
              }
              break;
            } case Expression::ExpressionType::VariableAccess:
              ptr = path.back().first;
              goto topOfConditionals;
              break;
            case Expression::ExpressionType::SubExpression: {
              SubExpression* subExpression = (SubExpression*)expression;
              if (firstTime(subExpression))
              {
                path.push_back({subExpression, &subExpression->expression});
                ptr = subExpression->expression.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::PreUnaryOperator: {
              PreUnaryOperator* preUnaryOperator = (PreUnaryOperator*)expression;
              if (firstTime(preUnaryOperator))
              {
                path.push_back({preUnaryOperator, &preUnaryOperator->operand});
                ptr = preUnaryOperator->operand.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::PostUnaryOperator: {
              PostUnaryOperator* postUnaryOperator = (PostUnaryOperator*)expression;
              if (firstTime(postUnaryOperator))
              {
                path.push_back({postUnaryOperator, &postUnaryOperator->operand});
                ptr = postUnaryOperator->operand.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::BinaryOperator: {
              BinaryOperator* binaryOperator = (BinaryOperator*)expression;
              if (firstTime(binaryOperator))
              {
                path.push_back({binaryOperator, &binaryOperator->leftOperand});
                ptr = binaryOperator->leftOperand.get();
              } else if (path.back().second == &binaryOperator->leftOperand)
              {
                path.back().second = &binaryOperator->rightOperand;
                ptr = binaryOperator->rightOperand.get();
              } else if (path.back().second == &binaryOperator->rightOperand)
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::TernaryOperator: {
              TernaryOperator* ternaryOperator = (TernaryOperator*)statement;
              if (firstTime(ternaryOperator))
              {
                path.push_back({ternaryOperator, &ternaryOperator->condition});
                ptr = ternaryOperator->condition.get();
              } else if (path.back().second == &ternaryOperator->condition)
              {
                path.back().second = &ternaryOperator->trueOperand;
                ptr = ternaryOperator->trueOperand.get();
              } else if (ternaryOperator->falseOperand && path.back().second == &ternaryOperator->trueOperand)
              {
                path.back().second = &ternaryOperator->falseOperand;
                ptr = ternaryOperator->falseOperand.get();
              } else if (!ternaryOperator->falseOperand || path.back().second == &ternaryOperator->falseOperand)
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } 
          }
          break;
        } case Statement::StatementType::Label:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Return: {
          Return* returnStatement = (Return*)statement;
          if (firstTime(returnStatement))
          {
            path.push_back({returnStatement, &returnStatement->data});
            ptr = returnStatement->data.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::Break:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Continue:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Goto:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Declaration: {
          Declaration* declaration = (Declaration*)statement;
          if (firstTime(declaration))
          {
            path.push_back({declaration, &declaration->dataType});
            ptr = declaration->dataType.get();
          } else if (declaration->value && path.back().second == &declaration->dataType)
          {
            path.back().second = &declaration->value;
            ptr = declaration->value.get();
          } else if (!declaration->value || path.back().second == &declaration->value)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::IfConditional: {
          IfConditional* ifConditional = (IfConditional*)statement;
          if (firstTime(ifConditional))
          {
            path.push_back({ifConditional, &ifConditional->condition});
            ptr = ifConditional->condition.get();
          } else if (path.back().second == &ifConditional->condition)
          {
            path.back().second = &ifConditional->body;
            ptr = ifConditional->body.get();
          } else if (ifConditional->elseStatement && path.back().second == &ifConditional->body)
          {
            path.back().second = &ifConditional->elseStatement;
            ptr = ifConditional->elseStatement.get();
          } else if (!ifConditional->elseStatement || path.back().second == &ifConditional->elseStatement)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::SwitchCase: {
          SwitchCase* switchCase = (SwitchCase*)statement;
          if (firstTime(switchCase))
          {
            path.push_back({switchCase, &switchCase->requirement});
            ptr = switchCase->requirement.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::SwitchDefault:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::SwitchConditional: {
          SwitchConditional* switchConditional = (SwitchConditional*)statement;
          if (firstTime(switchConditional))
          {
            path.push_back({switchConditional, &switchConditional->value});
            ptr = switchConditional->value.get();
          } else if (path.back().second == &switchConditional->value)
          {
            path.back().second = &switchConditional->body;
            ptr = switchConditional->body.get();
          } else if (path.back().second == &switchConditional->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::DoWhileLoop: {
          DoWhileLoop* doWhileLoop = (DoWhileLoop*)statement;
          if (firstTime(doWhileLoop))
          {
            path.push_back({doWhileLoop, &doWhileLoop->condition});
            ptr = doWhileLoop->condition.get();
          } else if (path.back().second == &doWhileLoop->condition)
          {
            path.back().second = &doWhileLoop->body;
            ptr = doWhileLoop->body.get();
          } else if (path.back().second == &doWhileLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::WhileLoop: {
          WhileLoop* whileLoop = (WhileLoop*)statement;
          if (firstTime(whileLoop))
          {
            path.push_back({whileLoop, &whileLoop->condition});
            ptr = whileLoop->condition.get();
          } else if (path.back().second == &whileLoop->condition)
          {
            path.back().second = &whileLoop->body;
            ptr = whileLoop->body.get();
          } else if (path.back().second == &whileLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::ForLoop:
          ForLoop* forLoop = (ForLoop*)statement;
          if (firstTime(forLoop))
          {
            path.push_back({forLoop, &forLoop->initialization});
            ptr = forLoop->initialization.get();
          } else if (path.back().second == &forLoop->initialization)
          {
            path.back().second = &forLoop->condition;
            ptr = forLoop->condition.get();
          } else if (path.back().second == &forLoop->condition)
          {
            path.back().second = &forLoop->update;
            ptr = forLoop->update.get();
          } else if (path.back().second == &forLoop->update)
          {
            path.back().second = &forLoop->body;
            ptr = forLoop->body.get();
          } else if (path.back().second == &forLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
      }
      break;
    } case ASTnode::NodeType::Program: {
      Program* program = (Program*)ptr;

      if (firstTime(program) && !program->nodes.empty())
      {
        path.push_back({program, &(program->nodes[0])});
        ptr = program->nodes[0].get();
        break;
      }

      for (std::vector<std::unique_ptr<ASTnode>>::iterator node = program->nodes.begin(); node != program->nodes.end(); node++)
      {
        if (&(*node) == path.back().second)
        {
          if (node+1 == program->nodes.end())
          {
            ptr = nullptr;
          } else
          {
            path.back().second = &(*(node+1));
            ptr = (node+1)->get();
          }
          break;
        }
      }
      break;
    } case ASTnode::NodeType::DataType:
      DataType* dataType = (DataType*)ptr;
      switch (dataType->generalType)
      {
        case DataType::GeneralType::PrimitiveType: {
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        } case DataType::GeneralType::Pointer: {
          Pointer* pointer = (Pointer*)dataType;
          if (firstTime(pointer))
          {
            path.push_back({pointer, &pointer->dataType});
            ptr = pointer->dataType.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case DataType::GeneralType::Function: {
          Function* function = (Function*)ptr;
          if (firstTime(function))
          {
            path.push_back({function, &function->returnType});
            ptr = function->returnType.get();
          } else if (path.back().second == &function->returnType)
          {
            if (function->parameters.empty())
            {
              /*if (functionDeclaration->body)
              {
                path.back().second = &functionDeclaration->body;
                ptr = functionDeclaration->body.get();
              } else
              {*/
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              //}
            } else
            {
              path.back().second = &function->parameters[0];
              ptr = function->parameters[0].get();
            }
          //} else if (path.back().second == &functionDeclaration->body)
          } else if (path.back().second == &function->parameters.back())
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          } else
          {
            for (std::vector<std::unique_ptr<Declaration>>::iterator node = function->parameters.begin(); node != function->parameters.end(); node++)
            {
              if (path.back().second == &(*node))
              {
                if (node+1 == function->parameters.end())
                {
                  /*if (function->body)
                  {
                    path.back().second = &function->body;
                    ptr = function->body.get();
                  } else
                  {*/
                    path.pop_back();
                    ptr = path.back().first;
                    goto topOfConditionals;
                  //}
                } else
                {
                  path.back().second = &(*(node+1));
                  ptr = (node+1)->get();
                }
                break;
              }
            }
          }
          break;
        } case DataType::GeneralType::Array: {
          Array* array = (Array*)dataType;
          if (firstTime(array))
          {
            path.push_back({array, &array->dataType});
            ptr = array->dataType.get();
          } else if (path.back().second == &array->dataType)
          {
            path.back().second = &array->size;
            ptr = array->size.get();
          } else if (path.back().second == &array->size)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case DataType::GeneralType::Struct:
          Struct* structure = (Struct*)ptr;
          if (firstTime(structure))
          {
            if (!structure->members.empty())
            {
              path.push_back({structure, &structure->members[0]});
              ptr = structure->members[0].first.get();
            } else
            {
              ptr = path.back().first;
              goto topOfConditionals;
            }
          } else if (path.back().second != &structure->members.back())
          {
            for (std::vector<std::pair<std::unique_ptr<Declaration>, uint8_t>>::iterator node = structure->members.begin(); node != structure->members.end(); node++)
            {
              if (path.back().second == &(*node))
              {
                path.back().second = &(*(node+1));
                ptr = (node+1)->first.get();
                break;
              }
            }
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
      }
      break;
  }

  if (path.empty())
  {
    ptr = nullptr;
  }

  return *this;
}

ASTiterator ASTiterator::operator++(int) {ASTiterator tmp = *this; ++(*this); return tmp;}

bool ASTiterator::firstTime(pointer ptr)
{
  return path.empty() || (ptr != path.back().first);
}


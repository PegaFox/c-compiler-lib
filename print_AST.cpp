#include "print_AST.hpp"

#include <iostream>

#include "parser/primitive_type.hpp"
#include "parser/pointer.hpp"
#include "parser/type_cast.hpp"

PrintAST::PrintAST(const Program& AST)
{
  std::cout << "Program {\n";

  for (const std::unique_ptr<ASTnode>& node : AST.nodes)
  {
    if (node->nodeType == ASTnode::NodeType::Statement)
    {
    if (((Statement*)node.get())->statementType == Statement::StatementType::FunctionDeclaration)
    {
      printFunctionDeclaration((FunctionDeclaration*)node.get());
    } else if (((Statement*)node.get())->statementType == Statement::StatementType::VariableDeclaration)
    {
      printVariableDeclaration((VariableDeclaration*)node.get());
    }
    }
  }

  std::cout << "}\n";
}

std::string PrintAST::depthPadding()
{
  return std::string(depth*2, ' ');
}

void PrintAST::printFunctionDeclaration(const FunctionDeclaration* functionDeclaration)
{
  depth++;

  std::cout << depthPadding() << "Function declaration {\n";
  depth++;
  std::cout << depthPadding() << "Return type: \"" << printDataType(functionDeclaration->returnType.get()) << "\"\n";
  std::cout << depthPadding() << "Identifier: \"" << functionDeclaration->identifier << "\"\n";

  std::cout << depthPadding() << "Parameters {\n";

  for (const std::unique_ptr<VariableDeclaration>& parameter: functionDeclaration->parameters)
  {
    printVariableDeclaration(parameter.get());
  }

  std::cout << depthPadding() << "}\n";

  if (functionDeclaration->body)
  {
    std::cout << depthPadding() << "Body {\n";

    printStatement(functionDeclaration->body.get());

    std::cout << depthPadding() << "}\n";
  }

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printStatement(const Statement* statement)
{
  switch (statement->statementType)
  {
    case Statement::StatementType::CompoundStatement:
      printCompoundStatement((CompoundStatement*)statement);
      break;
    case Statement::StatementType::Expression:
      printExpression((Expression*)statement);
      break;
    case Statement::StatementType::Return:
      printReturn((Return*)statement);
      break;
    case Statement::StatementType::Break:
      printBreak((Break*)statement);
      break;
    case Statement::StatementType::Continue:
      printContinue((Continue*)statement);
      break;
    case Statement::StatementType::Label:
      printLabel((Label*)statement);
      break;
    case Statement::StatementType::Goto:
      printGoto((Goto*)statement);
      break;
    case Statement::StatementType::VariableDeclaration:
      printVariableDeclaration((VariableDeclaration*)statement);
      break;
    case Statement::StatementType::SwitchCase:
      printSwitchCase((SwitchCase*)statement);
      break;
    case Statement::StatementType::SwitchDefault:
      printSwitchDefault((SwitchDefault*)statement);
      break;
    case Statement::StatementType::SwitchConditional:
      printSwitchConditional((SwitchConditional*)statement);
      break;
    case Statement::StatementType::IfConditional:
      printIfConditional((IfConditional*)statement);
      break;
    case Statement::StatementType::WhileLoop:
      printWhileLoop((WhileLoop*)statement);
      break;
    case Statement::StatementType::DoWhileLoop:
      printDoWhileLoop((DoWhileLoop*)statement);
      break;
    case Statement::StatementType::ForLoop:
      printForLoop((ForLoop*)statement);
      break;
  }
}

void PrintAST::printCompoundStatement(const CompoundStatement* compoundStatement)
{
  depth++;
  std::cout << depthPadding() << "Compound statement {\n";

  for (const std::unique_ptr<Statement>& statement : compoundStatement->body)
  {
    printStatement(statement.get());
  }

  std::cout << depthPadding() << "}\n";
  depth--;
}

void PrintAST::printVariableDeclaration(const VariableDeclaration* var)
{
  depth++;

  std::cout << depthPadding() << "Variable declaration {\n";
  depth++;

  std::cout << depthPadding() << "Data type: \"" << printDataType(var->dataType.get()) << "\"\n";
  std::cout << depthPadding() << "Identifier: \"" << var->identifier << "\"\n";
  if (var->value)
  {
    std::cout << depthPadding() << "Value {\n";
    printExpression(var->value.get());
    std::cout << depthPadding() << "}\n";
  }

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printIfConditional(const IfConditional* ifConditional)
{
  depth++;

  std::cout << depthPadding() << "If conditional {\n";
  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(ifConditional->condition.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Body {\n";
  printStatement(ifConditional->body.get());
  std::cout << depthPadding() << "}\n";

  if (ifConditional->elseStatement)
  {
    std::cout << depthPadding() << "Else {\n";
    printStatement(ifConditional->elseStatement.get());
    std::cout << depthPadding() << "}\n";
  }

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printWhileLoop(const WhileLoop* whileLoop)
{
  depth++;

  std::cout << depthPadding() << "While loop {\n";
  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(whileLoop->condition.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Body {\n";

  printStatement(whileLoop->body.get());
  
  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printSwitchCase(const SwitchCase* switchCase)
{
  depth++;

  std::cout << depthPadding() << "Switch case {\n";
  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(switchCase->requirement.get());
  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printSwitchDefault(const SwitchDefault* switchDefault)
{
  depth++;

  std::cout << depthPadding() << "Switch default\n";

  depth--;
}

void PrintAST::printSwitchConditional(const SwitchConditional* switchConditional)
{
  depth++;

  std::cout << depthPadding() << "Switch conditional {\n";
  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(switchConditional->value.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Body {\n";
  printStatement(switchConditional->body.get());
  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printDoWhileLoop(const DoWhileLoop* doWhileLoop)
{
  depth++;

  std::cout << depthPadding() << "Do while loop {\n";
  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(doWhileLoop->condition.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Body {\n";

  printStatement(doWhileLoop->body.get());
  
  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printForLoop(const ForLoop* forLoop)
{
  depth++;

  std::cout << depthPadding() << "For loop {\n";
  depth++;

  std::cout << depthPadding() << "Initialization {\n";
  if (forLoop->initialization)
  {
    printStatement(forLoop->initialization.get());
  } else
  {
    depth++;

    std::cout << depthPadding() << "Null Expression\n";

    depth--;
  }
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Condition {\n";
  printExpression(forLoop->condition.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Update {\n";
  printExpression(forLoop->update.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "Body {\n";

  printStatement(forLoop->body.get());

  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

std::string PrintAST::printDataType(const DataType* dataType)
{
  std::string typeString;

  switch (dataType->generalType)
  {
    case DataType::GeneralType::PrimitiveType: {
      const PrimitiveType* primitiveType = (PrimitiveType*)dataType;
      switch (primitiveType->type)
      {
        case PrimitiveType::Type::Void:
          typeString = "void";
          break;
        case PrimitiveType::Type::UnsignedChar:
          typeString = "unsigned char";
          break;
        case PrimitiveType::Type::SignedChar:
          typeString = "signed char";
          break;
        case PrimitiveType::Type::UnsignedShort:
          typeString = "unsigned short int";
          break;
        case PrimitiveType::Type::SignedShort:
          typeString = "signed short int";
          break;
        case PrimitiveType::Type::UnsignedInt:
          typeString = "unsigned int";
          break;
        case PrimitiveType::Type::SignedInt:
          typeString = "signed int";
          break;
        case PrimitiveType::Type::UnsignedLong:
          typeString = "unsigned long int";
          break;
        case PrimitiveType::Type::SignedLong:
          typeString = "signed long int";
          break;
        case PrimitiveType::Type::UnsignedLongLong:
          typeString = "unsigned long long int";
          break;
        case PrimitiveType::Type::SignedLongLong:
          typeString = "signed long long int";
          break;
        case PrimitiveType::Type::Float:
          typeString = "float";
          break;
        case PrimitiveType::Type::Double:
          typeString = "double";
          break;
        case PrimitiveType::Type::LongDouble:
          typeString = "long double";
          break;
      }
      break;
    } case DataType::GeneralType::Pointer:
      typeString = "pointer to " + printDataType(((Pointer*)dataType)->dataType.get());
      break;
    case DataType::GeneralType::Array:
      typeString = "array of X " + printDataType(((Pointer*)dataType)->dataType.get());
      break;
  }

  return typeString;
}

void PrintAST::printReturn(const Return* returnVal)
{
  depth++;

  std::cout << depthPadding() << "Return {\n";

  printExpression(returnVal->data.get());

  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printBreak(const Break* breakStatement)
{
  depth++;

  std::cout << depthPadding() << "Break\n";

  depth--;
}

void PrintAST::printContinue(const Continue* continueStatement)
{
  depth++;

  std::cout << depthPadding() << "Continue\n";

  depth--;
}

void PrintAST::printLabel(const Label* label)
{
  depth++;

  std::cout << depthPadding() << "Label: \"" << label->name << "\"\n";

  depth--;
}

void PrintAST::printGoto(const Goto* gotoStatement)
{
  depth++;

  std::cout << depthPadding() << "Goto: \"" << gotoStatement->label << "\"\n";

  depth--;
}

void PrintAST::printExpression(const Expression* expression)
{
  switch (expression->expressionType)
  {
    case Expression::ExpressionType::Null:
      depth++;

      std::cout << depthPadding() << "Null Expression\n";

      depth--;
      break;
    case Expression::ExpressionType::Constant:
      printConstant((Constant*)expression);
      break;
    case Expression::ExpressionType::VariableAccess:
      printVariableAccess((VariableAccess*)expression);
      break;
    case Expression::ExpressionType::FunctionCall:
      printFunctionCall((FunctionCall*)expression);
      break;
    case Expression::ExpressionType::SubExpression:
      printSubExpression((SubExpression*)expression);
      break;
    case Expression::ExpressionType::PreUnaryOperator:
      printPreUnaryOperator((PreUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::PostUnaryOperator:
      printPostUnaryOperator((PostUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::BinaryOperator:
      printBinaryOperator((BinaryOperator*)expression);
      break;
    case Expression::ExpressionType::TernaryOperator:
      printTernaryOperator((TernaryOperator*)expression);
      break;
  }
}

void PrintAST::printConstant(const Constant* constant)
{
  depth++;
  std::cout << depthPadding() << "Constant {\n";
  depth++;
  std::cout << depthPadding() << "Type: \"" << printDataType(constant->dataType.get()) << "\"\n";
  std::cout << depthPadding() << "Value: ";

  std::cout << (int)constant->value.unsignedChar << '\n';

  depth--;
  std::cout << depthPadding() << "}\n";
  depth--;
}

void PrintAST::printVariableAccess(const VariableAccess* variableAccess)
{
  depth++;
  std::cout << depthPadding() << "Variable access {\n";
  depth++;
  std::cout << depthPadding() << "Identifier: \"" << variableAccess->identifier << "\"\n";
  depth--;
  std::cout << depthPadding() << "}\n";
  depth--;
}

void PrintAST::printFunctionCall(const FunctionCall* functionCall)
{
  depth++;

  std::cout << depthPadding() << "Function call {\n";
  depth++;
  std::cout << depthPadding() << "Identifier: \"" << functionCall->identifier << "\"\n";

  std::cout << depthPadding() << "Parameters {\n";

  for (const std::unique_ptr<Expression>& argument: functionCall->arguments)
  {
    printExpression(argument.get());
  }

  std::cout << depthPadding() << "}\n";

  depth--;
  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printSubExpression(const SubExpression* subExpression)
{
  depth++;
  std::cout << depthPadding() << "Sub expression {\n";
  printExpression(subExpression->expression.get());
  std::cout << depthPadding() << "}\n";
  depth--;
}

void PrintAST::printPreUnaryOperator(const PreUnaryOperator* preUnary)
{
  depth++;

  switch (preUnary->preUnaryType)
  {
    case PreUnaryOperator::PreUnaryType::MathematicNegate:
      std::cout << depthPadding() << "Mathematic negate {\n";
      break;
    case PreUnaryOperator::PreUnaryType::BitwiseNOT:
      std::cout << depthPadding() << "Bitwise NOT {\n";
      break;
    case PreUnaryOperator::PreUnaryType::LogicalNegate:
      std::cout << depthPadding() << "Logical negate {\n";
      break;
    case PreUnaryOperator::PreUnaryType::Increment:
      std::cout << depthPadding() << "Pre increment {\n";
      break;
    case PreUnaryOperator::PreUnaryType::Decrement:
      std::cout << depthPadding() << "Pre decrement {\n";
      break;
    case PreUnaryOperator::PreUnaryType::Dereference:
      std::cout << depthPadding() << "Dereference {\n";
      break;
    case PreUnaryOperator::PreUnaryType::TypeCast:
      std::cout << depthPadding() << "Type cast {\n";
      depth++;
      std::cout << depthPadding() << "Type: \"" << printDataType(((TypeCast*)preUnary)->dataType.get()) << "\"\n";
      depth--;
      break;
    case PreUnaryOperator::PreUnaryType::Address:
      std::cout << depthPadding() << "Get address {\n";
      break;
  }

  printExpression(preUnary->operand.get());

  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printPostUnaryOperator(const PostUnaryOperator* postUnary)
{
  depth++;

  switch (postUnary->postUnaryType)
  {
    case PostUnaryOperator::PostUnaryType::Increment:
      std::cout << depthPadding() << "Post increment {\n";
      break;
    case PostUnaryOperator::PostUnaryType::Decrement:
      std::cout << depthPadding() << "Post decrement {\n";
      break;
  }

  printExpression(postUnary->operand.get());

  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printBinaryOperator(const BinaryOperator* binary)
{
  depth++;

  switch (binary->binaryType)
  {
    case BinaryOperator::BinaryType::Add:
      std::cout << depthPadding() << "Add {\n";
      break;
    case BinaryOperator::BinaryType::Subtract:
      std::cout << depthPadding() << "Subtract {\n";
      break;
    case BinaryOperator::BinaryType::Multiply:
      std::cout << depthPadding() << "Multiply {\n";
      break;
    case BinaryOperator::BinaryType::Divide:
      std::cout << depthPadding() << "Divide {\n";
      break;
    case BinaryOperator::BinaryType::Modulo:
      std::cout << depthPadding() << "Modulo {\n";
      break;
    case BinaryOperator::BinaryType::LeftShift:
      std::cout << depthPadding() << "Left shift {\n";
      break;
    case BinaryOperator::BinaryType::RightShift:
      std::cout << depthPadding() << "Right shift {\n";
      break;
    case BinaryOperator::BinaryType::BitwiseOR:
      std::cout << depthPadding() << "Bitwise OR {\n";
      break;
    case BinaryOperator::BinaryType::BitwiseAND:
      std::cout << depthPadding() << "Bitwise AND {\n";
      break;
    case BinaryOperator::BinaryType::BitwiseXOR:
      std::cout << depthPadding() << "Bitwise XOR {\n";
      break;
    case BinaryOperator::BinaryType::LogicalOR:
      std::cout << depthPadding() << "Logical OR {\n";
      break;
    case BinaryOperator::BinaryType::LogicalAND:
      std::cout << depthPadding() << "Logical AND {\n";
      break;
    case BinaryOperator::BinaryType::Subscript:
      std::cout << depthPadding() << "Array subscript {\n";
      break;
    case BinaryOperator::BinaryType::VariableAssignment:
      std::cout << depthPadding() << "Variable assignment {\n";
      break;
    case BinaryOperator::BinaryType::Equal:
      std::cout << depthPadding() << "Is equal {\n";
      break;
    case BinaryOperator::BinaryType::NotEqual:
      std::cout << depthPadding() << "Is not equal {\n";
      break;
    case BinaryOperator::BinaryType::Greater:
      std::cout << depthPadding() << "Is greater {\n";
      break;
    case BinaryOperator::BinaryType::Lesser:
      std::cout << depthPadding() << "Is lesser {\n";
      break;
    case BinaryOperator::BinaryType::GreaterOrEqual:
      std::cout << depthPadding() << "Is greater or equal {\n";
      break;
    case BinaryOperator::BinaryType::LesserOrEqual:
      std::cout << depthPadding() << "Is lesser or equal {\n";
      break;
  }

  printExpression(binary->leftOperand.get());

  printExpression(binary->rightOperand.get());

  std::cout << depthPadding() << "}\n";

  depth--;
}

void PrintAST::printTernaryOperator(const TernaryOperator* ternary)
{
  depth++;

  std::cout << depthPadding() << "Ternary operator {\n";

  depth++;

  std::cout << depthPadding() << "Condition {\n";
  printExpression(ternary->condition.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "True Operand {\n";
  printExpression(ternary->trueOperand.get());
  std::cout << depthPadding() << "}\n";

  std::cout << depthPadding() << "False Operand {\n";
  printExpression(ternary->falseOperand.get());
  std::cout << depthPadding() << "}\n";

  depth--;

  std::cout << depthPadding() << "}\n";

  depth--;
}

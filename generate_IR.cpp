#include "generate_IR.hpp"

#include <iostream>
#include <set>

struct IRGenError
{

};

extern bool optimize;

std::map<std::string, const FunctionDeclaration*> functions;

std::map<std::string, std::unique_ptr<DataType>> vars;

std::vector<const CompoundStatement*> scopes;

DataType* copyDataType(const DataType* data)
{
  DataType* result;

  switch (data->generalType)
  {
    case DataType::GeneralType::PrimitiveType:
      result = new PrimitiveType;
      ((PrimitiveType*)result)->type = ((PrimitiveType*)data)->type;
      break;
    case DataType::GeneralType::Pointer:
      result = new Pointer;
      ((Pointer*)result)->dataType.reset(copyDataType(((Pointer*)data)->dataType.get()));
      break;
    case DataType::GeneralType::Array:
      result = new Array;
      
      break;
    case DataType::GeneralType::Struct:

      break;
  }

  // set common base class info
  *result = *data;

  return result;
}

std::map<std::string, std::unique_ptr<DataType>>::iterator getIdentifier(const std::string& identifier)
{
  std::map<std::string, std::unique_ptr<DataType>>::iterator parentVar = vars.end();
  for (std::vector<const CompoundStatement*>::const_reverse_iterator scope = scopes.crbegin(); scope != scopes.crend(); scope++)
  {
    if (vars.contains("Scope_" + std::to_string((std::uintptr_t)*scope) + "_" + identifier))
    {
      parentVar = vars.find("Scope_" + std::to_string((std::uintptr_t)*scope) + "_" + identifier);
      break;
    }
  }

  if (parentVar == vars.end() && vars.contains(identifier))
  {
    parentVar = vars.find(identifier);
  } 

  if (parentVar != vars.end())
  {
    return parentVar; 
  } else
  {
    std::cout << "IR generation error: Variable \"" << identifier << "\" is not defined\n";
    throw IRGenError();
  }
}

std::vector<Operation> generateIR(const Program& AST)
{
  std::vector<Operation> absProgram;

  for (const std::unique_ptr<ASTnode>& node : AST.nodes)
  {
    if (node->nodeType == ASTnode::NodeType::Statement)
    {
      if (((Statement*)node.get())->statementType == Statement::StatementType::FunctionDeclaration)
      {
        generateFunctionDeclaration(absProgram, (FunctionDeclaration*)node.get());
      } else if (((Statement*)node.get())->statementType == Statement::StatementType::VariableDeclaration)
      {
        generateVariableDeclaration(absProgram, (VariableDeclaration*)node.get());
      }
    }
  }

  return absProgram;
}

void generateFunctionDeclaration(std::vector<Operation>& absProgram, const FunctionDeclaration* functionDeclaration)
{
  if (
    functions.contains(functionDeclaration->identifier) && 
    functionDeclaration->parameters.size() != functions[functionDeclaration->identifier]->parameters.size())
  {
    std::cout << "IR generation error: Wrong type for function \"" << functionDeclaration->identifier << "\"\n";
    throw IRGenError();
  }

  std::string name = (scopes.empty() ? "" : "Scope_" + std::to_string((std::uintptr_t)scopes.back()) + "_") + functionDeclaration->identifier;
  if (vars.contains(name) && vars[name])
  {
    std::cout << "IR generation error: Identifier \"" << functionDeclaration->identifier << "\" is already defined\n";
    throw IRGenError();
  } else
  {
    vars[name];
  }

  if (functionDeclaration->body)
  {
    if (!scopes.empty())
    {
      std::cout << "IR generation error: Function definitions are only allowed at global scope\n";
      throw IRGenError();
    }

    if (functions.contains(functionDeclaration->identifier) && functions[functionDeclaration->identifier]->body)
    {
      std::cout << "IR generation error: Function \"" << functionDeclaration->identifier << "\" already exists\n";
      throw IRGenError();
    } else
    {
      functions[functionDeclaration->identifier] = functionDeclaration;
    }
  }

  if (!functions.contains(functionDeclaration->identifier))
  {
    functions[functionDeclaration->identifier] = functionDeclaration;
  } 

  if (functionDeclaration->body)
  {
    scopes.emplace_back(functionDeclaration->body.get());

    for (const std::unique_ptr<VariableDeclaration>& parameter: functionDeclaration->parameters)
    {
      generateVariableDeclaration(absProgram, parameter.get(), false);
    }

    absProgram.emplace_back(Operation{Operation::Label, {functionDeclaration->identifier + "_Function"}});
    generateStatement(absProgram, functionDeclaration->body.get());
    absProgram.emplace_back(Operation{Operation::Return});

    scopes.pop_back();
  } else
  {
    std::set<std::string> params;
    for (const std::unique_ptr<VariableDeclaration>& parameter: functionDeclaration->parameters)
    {
      if (params.contains(parameter->identifier))
      {
        std::cout << "IR generation error: duplicate function parameter names\n";
        throw IRGenError();
      }
      params.insert(parameter->identifier);
    }
  }
}

void generateStatement(std::vector<Operation>& absProgram, const Statement* statement)
{
  /*
  At the beginning of each statement:
  - The value of ACC is undefined
  - The value of PC is undefined
  - The value of PTR is undefined
  - The value of BUS is 0xF
  - The value of the stack pointer is at memory location 0x00
  - The bottom of the stack is at memory location 0xEF
  - Memory locations 0xF0 through 0xF7 are reserved as registers

  RAM optimizations
  - Don't setup stack unless it is used in the program (done)

  ROM optimizations
  - Precompute constant expressions (done)
  - If accessing a variable with a constant location, precompute the location to access
  - If storing a value in a register is not necessary, store it in ACC
  - Remove useless instructions like changing bus twice in a row
  */

  switch (statement->statementType)
  {
    case Statement::StatementType::CompoundStatement:
      generateCompoundStatement(absProgram, (CompoundStatement*)statement);
      break;
    case Statement::StatementType::Expression:
      generateExpression(absProgram, (Expression*)statement);
      break;
    case Statement::StatementType::Return:
      generateReturn(absProgram, (Return*)statement);
      break;
    case Statement::StatementType::Break:
      generateBreak(absProgram, (Break*)statement);
      break;
    case Statement::StatementType::Continue:
      generateContinue(absProgram, (Continue*)statement);
      break;
    case Statement::StatementType::Label:
      generateLabel(absProgram, (Label*)statement);
      break;
    case Statement::StatementType::Goto:
      generateGoto(absProgram, (Goto*)statement);
      break;
    case Statement::StatementType::VariableDeclaration:
      generateVariableDeclaration(absProgram, (VariableDeclaration*)statement);
      break;
    case Statement::StatementType::FunctionDeclaration:
      generateFunctionDeclaration(absProgram, (FunctionDeclaration*)statement);
      break;
    case Statement::StatementType::IfConditional:
      generateIfConditional(absProgram, (IfConditional*)statement);
      break;
    case Statement::StatementType::SwitchCase:
      generateSwitchCase(absProgram, (SwitchCase*)statement);
      break;
    case Statement::StatementType::SwitchDefault:
      generateSwitchDefault(absProgram, (SwitchDefault*)statement);
      break;
    case Statement::StatementType::SwitchConditional:
      generateSwitchConditional(absProgram, (SwitchConditional*)statement);
      break;
    case Statement::StatementType::DoWhileLoop:
      generateDoWhileLoop(absProgram, (DoWhileLoop*)statement);
      break;
    case Statement::StatementType::WhileLoop:
      generateWhileLoop(absProgram, (WhileLoop*)statement);
      break;
    case Statement::StatementType::ForLoop:
      generateForLoop(absProgram, (ForLoop*)statement);
      break;
  }
}

void generateCompoundStatement(std::vector<Operation>& absProgram, const CompoundStatement* compoundStatement)
{
  // this check allows statements like for loops to impose the scope prematurely without causing problems
  scopes.emplace_back(compoundStatement);
  for (const std::unique_ptr<Statement>& statement : compoundStatement->body)
  {
    generateStatement(absProgram, statement.get());
  }
  scopes.pop_back();
}

// returns the variable name containing the result of the expression
std::string generateExpression(std::vector<Operation>& absProgram, const Expression* expression)
{
  switch (expression->expressionType)
  {
    case Expression::ExpressionType::Constant:
      return generateConstant(absProgram, (Constant*)expression);
      break;
    case Expression::ExpressionType::VariableAccess:
      return generateVariableAccess(absProgram, (VariableAccess*)expression);
      break;
    case Expression::ExpressionType::FunctionCall:
      return generateFunctionCall(absProgram, (FunctionCall*)expression);
      break;
    case Expression::ExpressionType::SubExpression:
      return generateExpression(absProgram, ((SubExpression*)expression)->expression.get());
      break;
    case Expression::ExpressionType::PreUnaryOperator:
      return generatePreUnaryOperator(absProgram, (PreUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::PostUnaryOperator:
      return generatePostUnaryOperator(absProgram, (PostUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::BinaryOperator:
      return generateBinaryOperator(absProgram, (BinaryOperator*)expression);
      break;
    case Expression::ExpressionType::TernaryOperator:
      return generateTernaryOperator(absProgram, (TernaryOperator*)expression);
      break;
  }
  return "";
}

std::string generateConstant(std::vector<Operation>& absProgram, const Constant* constant)
{
  // number comes first to ensure that the variable name is unique (identifiers can't start with numbers in C)
  if (constant->dataType->generalType == DataType::GeneralType::PrimitiveType)
  {
    switch (((PrimitiveType*)constant->dataType.get())->type)
    {
      case PrimitiveType::Type::SignedChar:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.signedChar)}});
        break;
      case PrimitiveType::Type::UnsignedChar:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.unsignedChar)}});
        break;
      case PrimitiveType::Type::SignedShort:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.signedShort)}});
        break;
      case PrimitiveType::Type::UnsignedShort:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.unsignedShort)}});
        break;
      case PrimitiveType::Type::SignedInt:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.signedInt)}});
        break;
      case PrimitiveType::Type::UnsignedInt:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.unsignedInt)}});
        break;
      case PrimitiveType::Type::SignedLong:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.signedLong)}});
        break;
      case PrimitiveType::Type::UnsignedLong:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.unsignedLong)}});
        break;
      case PrimitiveType::Type::SignedLongLong:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.signedLongLong)}});
        break;
      case PrimitiveType::Type::UnsignedLongLong:
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(constant->value.unsignedLongLong)}});
        break;
    }
  }
  
  return absProgram.back().operands[0];
}

// return is currently equivalent to exit(0). this should be fixed when functions are added
void generateReturn(std::vector<Operation>& absProgram, const Return* returnVal)
{
  std::string name = generateExpression(absProgram, returnVal->data.get());

  absProgram.emplace_back(Operation{Operation::Return, {name}});
}

void generateBreak(std::vector<Operation>& absProgram, const Break* breakStatement)
{
  bool found = false;
  uint32_t depth = 0;

  for (std::vector<Operation>::const_reverse_iterator op = absProgram.rbegin(); op != absProgram.rend(); op++)
  {
    if (
      op->code == Operation::Label)
    {
      if (
        op->operands[0].find("_SwitchConditionalBegin") != std::string::npos ||
        op->operands[0].find("_DoWhileLoopBegin") != std::string::npos ||
        op->operands[0].find("_WhileLoopBegin") != std::string::npos ||
        op->operands[0].find("_ForLoopBegin") != std::string::npos)
      {
        if (depth == 0)
        {
          absProgram.emplace_back(Operation{Operation::Jump, {op->operands[0].substr(0, op->operands[0].size()-5) + "End"}});
          found = true;
          break;
        } else
        {
          depth--;
        }
      } else if (
        op->operands[0].find("_SwitchConditionalEnd") != std::string::npos ||
        op->operands[0].find("_DoWhileLoopEnd") != std::string::npos ||
        op->operands[0].find("_WhileLoopEnd") != std::string::npos ||
        op->operands[0].find("_ForLoopEnd") != std::string::npos)
      {
        depth++;
      }
    }
  }

  if (!found)
  {
    std::cout << "IR generation error: Unexpected break statement\n";
    throw IRGenError();
  }
}

void generateContinue(std::vector<Operation>& absProgram, const Continue* continueStatement)
{
  bool found = false;
  uint32_t depth = 0;

  for (std::vector<Operation>::const_reverse_iterator op = absProgram.rbegin(); op != absProgram.rend(); op++)
  {
    if (
      op->code == Operation::Label)
    {
      if (
        op->operands[0].find("_DoWhileLoopBegin") != std::string::npos ||
        op->operands[0].find("_WhileLoopBegin") != std::string::npos ||
        op->operands[0].find("_ForLoopBegin") != std::string::npos)
      {
        if (depth == 0)
        {
          absProgram.emplace_back(Operation{Operation::Jump, {op->operands[0]}});
          found = true;
          break;
        } else
        {
          depth--;
        }
      } else if (
        op->operands[0].find("_DoWhileLoopEnd") != std::string::npos ||
        op->operands[0].find("_WhileLoopEnd") != std::string::npos ||
        op->operands[0].find("_ForLoopEnd") != std::string::npos)
      {
        depth++;
      }
    }
  }

  if (!found)
  {
    std::cout << "IR generation error: Unexpected continue statement\n";
    throw IRGenError();
  }
}

void generateLabel(std::vector<Operation>& absProgram, const Label* label)
{
  absProgram.emplace_back(Operation{Operation::Label, {label->name}});
}

void generateGoto(std::vector<Operation>& absProgram, const Goto* gotoStatement)
{
  absProgram.emplace_back(Operation{Operation::Jump, {gotoStatement->label}});
}

void generateVariableDeclaration(std::vector<Operation>& absProgram, const VariableDeclaration* variableDeclaration, bool allowInitialization)
{
  std::string name = variableDeclaration->identifier;
  if (!scopes.empty())
  {
    name = "Scope_" + std::to_string((std::uintptr_t)scopes.back()) + "_" + name;
  }

  if (vars.contains(name))
  {
    std::cout << "IR generation error: Variable \"" << variableDeclaration->identifier << "\" already declared\n";
    throw IRGenError();
  }

  if (functions.contains(name))
  {
    std::cout << "IR generation error: Identifier \"" << variableDeclaration->identifier << "\" is already defined\n";
    throw IRGenError();
  }

  vars[name].reset(copyDataType(variableDeclaration->dataType.get()));
  
  if (variableDeclaration->value)
  {
    if (!allowInitialization)
    {
      std::cout << "IR generation error: Variable \"" << variableDeclaration->identifier << "\" cannot be initialized here\n";
      throw IRGenError();
    }
    std::string outputName = generateExpression(absProgram, variableDeclaration->value.get());
    absProgram.emplace_back(Operation{Operation::Set, {name, outputName}});
  }
}

void generateIfConditional(std::vector<Operation>& absProgram, const IfConditional* ifConditional)
{
  scopes.emplace_back((CompoundStatement*)ifConditional);
  
  // conditional jump to end of if conditional
  std::string name = generateExpression(absProgram, ifConditional->condition.get());

  absProgram.emplace_back(Operation{Operation::JumpIfZero, {std::to_string((std::uintptr_t)ifConditional) + "_IfConditionalEnd", name}});

  // if conditional body
  generateStatement(absProgram, ifConditional->body.get());

  if (ifConditional->elseStatement)
  {
    // unconditional jump to end of else conditional
    absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)ifConditional) + "_ElseConditionalEnd"}});
  }

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)ifConditional) + "_IfConditionalEnd"}});
  
  if (ifConditional->elseStatement)
  {
    // else body
    generateStatement(absProgram, ifConditional->elseStatement.get());

    absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)ifConditional) + "_ElseConditionalEnd"}});
  }

  scopes.pop_back();
}

void generateSwitchCase(std::vector<Operation>& absProgram, const SwitchCase* switchCase)
{
  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)switchCase) + "_SwitchCase"}});
}

void generateSwitchDefault(std::vector<Operation>& absProgram, const SwitchDefault* switchDefault)
{
  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)switchDefault) + "_SwitchDefault"}});
}

void generateSwitchConditional(std::vector<Operation>& absProgram, const SwitchConditional* switchConditional)
{
  /*
  if (condition == case1)
  {
    jump case1
  }
  if (condition == case2)
  {
    jump case2
  }
  jump default
  */
 
  scopes.emplace_back((CompoundStatement*)switchConditional);

  // this label is never actually jumped to, it just tells the break statement what it's inside of
  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalBegin"}});

  std::string name = generateExpression(absProgram, switchConditional->value.get());

  const SwitchDefault* defaultCase = nullptr;

  for (ASTiterator node((ASTnode*)switchConditional); node != ASTiterator(nullptr); node++)
  {
    if (node->nodeType == ASTnode::NodeType::Statement)
    {
      const Statement* statement = (Statement*)node.ptr;
      switch (statement->statementType)
      {
        case Statement::StatementType::SwitchConditional: {
          if (statement == switchConditional)
          {
            break;
          }

          std::size_t depth = node.depth();
          while (node.depth() >= depth)
          {
            node++;
          }
          break;
        } case Statement::StatementType::SwitchCase: {
          std::string caseName = generateExpression(absProgram, ((SwitchCase*)statement)->requirement.get());

          absProgram.emplace_back(Operation{Operation::SetSubtraction, {std::to_string((std::uintptr_t)statement) + "_Subtracted", name, caseName}});
          absProgram.emplace_back(Operation{Operation::JumpIfZero, {std::to_string((std::uintptr_t)statement) + "_SwitchCase", std::to_string((std::uintptr_t)statement) + "_Subtracted"}});
          break;
        } case Statement::StatementType::SwitchDefault:
          defaultCase = (SwitchDefault*)statement;
          break;
        default:
          break;
      }
    }
  }

  // handle default case here
  if (defaultCase)
  {
    absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)defaultCase) + "_SwitchDefault"}});
  } else
  {
    absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalEnd"}});
  }

  generateStatement(absProgram, switchConditional->body.get());

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalEnd"}});

  scopes.pop_back();
}

void generateDoWhileLoop(std::vector<Operation>& absProgram, const DoWhileLoop* doWhileLoop)
{
  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopBegin"}});

  generateStatement(absProgram, doWhileLoop->body.get());

  std::string name = generateExpression(absProgram, doWhileLoop->condition.get());

  absProgram.emplace_back(Operation{Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopBegin", name}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopEnd"}});
}

void generateWhileLoop(std::vector<Operation>& absProgram, const WhileLoop* whileLoop)
{
  scopes.emplace_back((CompoundStatement*)whileLoop);

  absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopCondition"}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopBegin"}});

  generateStatement(absProgram, whileLoop->body.get());

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopCondition"}});

  std::string name = generateExpression(absProgram, whileLoop->condition.get());

  absProgram.emplace_back(Operation{Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopBegin", name}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopEnd"}});

  scopes.pop_back();
}

void generateForLoop(std::vector<Operation>& absProgram, const ForLoop* forLoop)
{
  scopes.emplace_back((CompoundStatement*)forLoop);
  if (forLoop->initialization)
  {
    generateStatement(absProgram, forLoop->initialization.get());
  }

  absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopCondition"}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin"}});

  generateStatement(absProgram, forLoop->body.get());

  if (forLoop->update)
  {
    generateStatement(absProgram, forLoop->update.get());
  }

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopCondition"}});

  if (forLoop->condition) // condition defaults to constant 1 if not specified
  {
    std::string name = generateExpression(absProgram, forLoop->condition.get());

    absProgram.emplace_back(Operation{Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin", name}});
  } else
  {
    absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin"}});
  }

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopEnd"}});

  scopes.pop_back();
}

std::string generateVariableAccess(std::vector<Operation>& absProgram, const VariableAccess* variableAccess)
{
  std::map<std::string, std::unique_ptr<DataType>>::iterator name = getIdentifier(variableAccess->identifier);
  if (name->second)
  {
    return name->first;
  } else
  {
    std::cout << "IR generation error: \"" << variableAccess->identifier << "\" is not a variable\n";
    throw IRGenError();
  }
}

std::string generateFunctionCall(std::vector<Operation>& absProgram, const FunctionCall* functionCall)
{
  if (functions.contains(functionCall->identifier) && !getIdentifier(functionCall->identifier)->second)
  {
    if (functionCall->arguments.size() != functions[functionCall->identifier]->parameters.size())
    {
      std::cout << "IR generation error: Wrong argument count for function \"" << functionCall->identifier << "\". Expected " << functions[functionCall->identifier]->parameters.size() << ", received " << functionCall->arguments.size() << "\n";
      throw IRGenError();
    }

    for (const std::unique_ptr<Expression>& argument: functionCall->arguments)
    {
      std::string name = generateExpression(absProgram, argument.get());
      absProgram.emplace_back(Operation{Operation::AddArg, {name}});
    }
    absProgram.emplace_back(Operation{Operation::Call, {std::to_string((uintptr_t)functionCall) + "_ReturnValue", functionCall->identifier}});
    return std::to_string((uintptr_t)functionCall) + "_ReturnValue";
  } else
  {
    std::cout << "IR generation error: Function \"" << functionCall->identifier << "\" is not defined\n";
    throw IRGenError();
  }
}

std::string generatePreUnaryOperator(std::vector<Operation>& absProgram, const PreUnaryOperator* preUnary)
{
  std::string name = generateExpression(absProgram, preUnary->operand.get());

  switch (preUnary->preUnaryType)
  {
    case PreUnaryOperator::PreUnaryType::Address:
      if (vars.contains(name))
      {
        absProgram.emplace_back(Operation{Operation::GetAddress, {std::to_string((uintptr_t)preUnary) + "_Address", name}});
      } else
      {
        std::cout << "IR generation error: Getting address of rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case PreUnaryOperator::PreUnaryType::Dereference:
      absProgram.emplace_back(Operation{Operation::Dereference, {std::to_string((uintptr_t)preUnary) + "_Dereference", name}});
      break;
    case PreUnaryOperator::PreUnaryType::TypeCast:
      absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)preUnary) + (((TypeCast*)preUnary)->dataType->isVolatile ? "_Volatile" : "") + "_Casted", name}});
      break;
    case PreUnaryOperator::PreUnaryType::MathematicNegate:
      absProgram.emplace_back(Operation{Operation::Negate, {std::to_string((uintptr_t)preUnary) + "_Negated", name}});
      break;
    case PreUnaryOperator::PreUnaryType::BitwiseNOT:
      absProgram.emplace_back(Operation{Operation::BitwiseNOT, {std::to_string((uintptr_t)preUnary) + "_BitwiseNegated", name}});
      break;
    case PreUnaryOperator::PreUnaryType::LogicalNegate:
      absProgram.emplace_back(Operation{Operation::LogicalNOT, {std::to_string((uintptr_t)preUnary) + "_LogicalNegated", name}});
      break;
    case PreUnaryOperator::PreUnaryType::Increment: {
      if (vars.contains(name))
      {
        absProgram.emplace_back(Operation{Operation::SetAddition, {name, name, "1"}});
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)preUnary) + "_Incremented", name}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PreUnaryOperator::PreUnaryType::Decrement:
      if (vars.contains(name))
      {
        absProgram.emplace_back(Operation{Operation::SetSubtraction, {name, name, "1"}});
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)preUnary) + "_Decremented", name}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return absProgram.back().operands[0];
}

std::string generatePostUnaryOperator(std::vector<Operation>& absProgram, const PostUnaryOperator* postUnary)
{
  std::string name = generateExpression(absProgram, postUnary->operand.get());

  switch (postUnary->postUnaryType)
  {
    case PostUnaryOperator::PostUnaryType::Increment: {
      if (vars.contains(name))
      {
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)postUnary) + "_PostIncremented", name}});
        absProgram.emplace_back(Operation{Operation::SetAddition, {name, name, "1"}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PostUnaryOperator::PostUnaryType::Decrement:
      if (vars.contains(name))
      {
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)postUnary) + "_PostDecremented", name}});
        absProgram.emplace_back(Operation{Operation::SetSubtraction, {name, name, "1"}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return absProgram.end()[-2].operands[0];
}

std::string generateBinaryOperator(std::vector<Operation>& absProgram, const BinaryOperator* binary)
{
  std::string leftOperandName = generateExpression(absProgram, binary->leftOperand.get());
  std::string rightOperandName = generateExpression(absProgram, binary->rightOperand.get());

  switch (binary->binaryType)
  {
    case BinaryOperator::BinaryType::VariableAssignment:
      if (vars.contains(leftOperandName) || leftOperandName.find("_Dereference") != std::string::npos)
      {
        absProgram.emplace_back(Operation{Operation::Set, {leftOperandName, rightOperandName}});
        absProgram.emplace_back(Operation{Operation::Set, {std::to_string((uintptr_t)binary) + "_Assigned", leftOperandName}});
      } else
      {
        std::cout << "IR generation error: Assigning rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case BinaryOperator::BinaryType::Add:
      absProgram.emplace_back(Operation{Operation::SetAddition, {std::to_string((uintptr_t)binary) + "_Added", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Subtract:
      absProgram.emplace_back(Operation{Operation::SetSubtraction, {std::to_string((uintptr_t)binary) + "_Subtracted", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Multiply:
      absProgram.emplace_back(Operation{Operation::SetMultiplication, {std::to_string((uintptr_t)binary) + "_Multiplied", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Divide:
      absProgram.emplace_back(Operation{Operation::SetDivision, {std::to_string((uintptr_t)binary) + "_Divided", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Modulo:
      absProgram.emplace_back(Operation{Operation::SetModulo, {std::to_string((uintptr_t)binary) + "_Moduloed", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::LeftShift:
      absProgram.emplace_back(Operation{Operation::SetLeftShift, {std::to_string((uintptr_t)binary) + "_LeftShifted", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::RightShift:
      absProgram.emplace_back(Operation{Operation::SetRightShift, {std::to_string((uintptr_t)binary) + "_RightShifted", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::BitwiseOR:
      absProgram.emplace_back(Operation{Operation::SetBitwiseOR, {std::to_string((uintptr_t)binary) + "_BitwiseOred", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::BitwiseAND:
      absProgram.emplace_back(Operation{Operation::SetBitwiseAND, {std::to_string((uintptr_t)binary) + "_BitwiseAnded", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::BitwiseXOR:
      absProgram.emplace_back(Operation{Operation::SetBitwiseXOR, {std::to_string((uintptr_t)binary) + "_BitwiseXored", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::LogicalOR:
      absProgram.emplace_back(Operation{Operation::SetLogicalOR, {std::to_string((uintptr_t)binary) + "_LogicalOred", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::LogicalAND:
      absProgram.emplace_back(Operation{Operation::SetLogicalAND, {std::to_string((uintptr_t)binary) + "_LogicalAnded", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Subscript:

      break;
    case BinaryOperator::BinaryType::Equal:
      absProgram.emplace_back(Operation{Operation::SetEqual, {std::to_string((uintptr_t)binary) + "_Equaled", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::NotEqual:
      absProgram.emplace_back(Operation{Operation::SetNotEqual, {std::to_string((uintptr_t)binary) + "_NotEqualed", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Greater:
      absProgram.emplace_back(Operation{Operation::SetGreater, {std::to_string((uintptr_t)binary) + "_Large", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::Lesser:
      absProgram.emplace_back(Operation{Operation::SetLesser, {std::to_string((uintptr_t)binary) + "_Small", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::GreaterOrEqual:
      absProgram.emplace_back(Operation{Operation::SetGreaterOrEqual, {std::to_string((uintptr_t)binary) + "_Largeish", leftOperandName, rightOperandName}});
      break;
    case BinaryOperator::BinaryType::LesserOrEqual:
      absProgram.emplace_back(Operation{Operation::SetLesserOrEqual, {std::to_string((uintptr_t)binary) + "_Smallish", leftOperandName, rightOperandName}});
      break;
  }

  return absProgram.back().operands[0];
}

std::string generateTernaryOperator(std::vector<Operation>& absProgram, const TernaryOperator* ternary)
{
  // conditional jump to end of if conditional
  std::string name = generateExpression(absProgram, ternary->condition.get());

  absProgram.emplace_back(Operation{Operation::JumpIfZero, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorFalse", name}});

  // if conditional body
  std::string trueName = generateExpression(absProgram, ternary->trueOperand.get());
  absProgram.emplace_back(Operation{Operation::Set, {name, trueName}});

  // unconditional jump to end of else conditional
  absProgram.emplace_back(Operation{Operation::Jump, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorEnd"}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorFalse"}});

  // else body
  std::string falseName = generateExpression(absProgram, ternary->falseOperand.get());
  absProgram.emplace_back(Operation{Operation::Set, {name, falseName}});

  absProgram.emplace_back(Operation{Operation::Label, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorEnd"}});

  return name;
}

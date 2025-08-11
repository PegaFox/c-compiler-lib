#include "generate_IR.hpp"

#include <iostream>
#include <set>

#include "parser/AST_iterator.hpp"
#include "parser/primitive_type.hpp"
#include "parser/pointer.hpp"
#include "parser/array.hpp"
#include "parser/struct.hpp"
#include "parser/type_cast.hpp"
#include "parser/sub_expression.hpp"

/*DataType* GenerateIR::copyDataType(const DataType* data)
{
  DataType* result;

  switch (data->generalType)
  {
    case DataType::GeneralType::PrimitiveType:
      result = new PrimitiveType;
      *((PrimitiveType*)result) = *((PrimitiveType*)data);
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
}*/

std::vector<Operation> GenerateIR::generateIR(const Program& AST)
{
  CommonIRData data;

  for (const std::unique_ptr<ASTnode>& node : AST.nodes)
  {
    if (node->nodeType == ASTnode::NodeType::Statement)
    {
      if (((Statement*)node.get())->statementType == Statement::StatementType::Declaration)
      {
        generateDeclaration(data, (Declaration*)node.get());
      }
    }
  }

  return data.irProgram;
}

void GenerateIR::optimizeIR(std::vector<Operation> &irProgram)
{
  bool changed;

  do
  {
    changed = false;

    changed |= resolveConstantOperations(irProgram);
    changed |= trimInaccessibleCode(irProgram);
  } while (changed);
}

Operation::DataType GenerateIR::ASTTypeToIRType(CommonIRData& data, DataType* dataType)
{
  switch (dataType->generalType)
  {
    case DataType::GeneralType::PrimitiveType: {
      PrimitiveType* primitiveType = (PrimitiveType*)dataType;

      return {primitiveType->size, primitiveType->size, 0, primitiveType->isSigned, primitiveType->isFloating};
    } case DataType::GeneralType::Pointer: {
      Pointer* pointer = (Pointer*)dataType;
      
      Operation::DataType childType = ASTTypeToIRType(data, pointer->dataType.get());
      childType.pointerDepth++;
      return childType;
    } case DataType::GeneralType::Array: {
      Array* array = (Array*)dataType;

      // We can't store both the size of the array, and the size of each member, but since arrays are always lvalues (using them as rvalues casts them to pointers) we can fetch the size of each element from the declarations map and store the total structure size here.
      Operation::DataType childType = ASTTypeToIRType(data, array->dataType.get());

      if (array->size->expressionType == Expression::ExpressionType::Constant)
      {
        childType.size *= (uint64_t)((Constant*)array->size.get())->value;
      } else
      {
        childType.size = 0;
      }
      
      childType.pointerDepth++;
      return childType;
    } case DataType::GeneralType::Struct: {
      Struct* structure = (Struct*)dataType;

      Operation::DataType result;
      for (std::pair<std::unique_ptr<Declaration>, uint8_t>& member: structure->members)
      {
        Operation::DataType memberType;
        if (member.first->dataType->generalType != DataType::GeneralType::Pointer)
        {
          memberType = ASTTypeToIRType(data, member.first->dataType.get());
        } else
        {
          memberType = {data.pointerSize, data.pointerSize};
        }

        result.alignment = std::max(result.alignment, memberType.alignment);
        result.size += memberType.alignment - result.size%memberType.alignment;
        result.size += memberType.size;
      }

      return result;
    } default:
      std::cout << "IR generation error: Invalid data type\n";
      throw IRGenError();
  }
}

std::map<std::string, const Declaration*>::iterator GenerateIR::getIdentifier(const std::string& identifier)
{
  std::map<std::string, const Declaration*>::iterator parentVar = declarations.end();
  for (std::vector<const CompoundStatement*>::const_reverse_iterator scope = scopes.crbegin(); scope != scopes.crend(); scope++)
  {
    if (declarations.contains("Scope_" + std::to_string((std::uintptr_t)*scope) + "_" + identifier))
    {
      parentVar = declarations.find("Scope_" + std::to_string((std::uintptr_t)*scope) + "_" + identifier);
      break;
    }
  }

  if (parentVar == declarations.end() && declarations.contains(identifier))
  {
    parentVar = declarations.find(identifier);
  } 

  if (parentVar != declarations.end())
  {
    return parentVar; 
  } else
  {
    std::cout << "IR generation error: Variable \"" << identifier << "\" is not defined\n";
    throw IRGenError();
  }
}

/*void generateFunctionDeclaration(CommonIRData& data, const FunctionDeclaration* functionDeclaration)
{
  if (
    functions.contains(functionDeclaration->identifier) && 
    functionDeclaration->parameters.size() != functions[functionDeclaration->identifier]->parameters.size())
  {
    std::cout << "IR generation error: Wrong type for function \"" << functionDeclaration->identifier << "\"\n";
    throw IRGenError();
  }

  std::pair<std::string, PrimitiveType> name = (scopes.empty() ? "" : "Scope_" + std::to_string((std::uintptr_t)scopes.back()) + "_") + functionDeclaration->identifier;
  if (vars.contains(name) && vars[name])
  {
    std::cout << "IR generation error: Identifier \"" << functionDeclaration->identifier << "\" is already defined\n";
    throw IRGenError();
  } else
  {
    vars[name];
  }

  if (functionDeclaration->returnType->linkage == DataType::Linkage::Internal && !scopes.empty())
  {
    std::cout << "IR generation error: Local function declarations cannot be static\n";
    throw IRGenError();
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
      generateVariableDeclaration(data, parameter.get(), false);
    }

    data.irProgram.emplace_back(Operation{Operation::Label, {functionDeclaration->identifier + "_Function"}});
    generateStatement(data, functionDeclaration->body.get());
    data.irProgram.emplace_back(Operation{Operation::Return});

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
}*/

void GenerateIR::generateStatement(CommonIRData& data, const Statement* statement)
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
      generateCompoundStatement(data, (CompoundStatement*)statement);
      break;
    case Statement::StatementType::Expression:
      generateExpression(data, (Expression*)statement);
      break;
    case Statement::StatementType::Return:
      generateReturn(data, (Return*)statement);
      break;
    case Statement::StatementType::Break:
      generateBreak(data, (Break*)statement);
      break;
    case Statement::StatementType::Continue:
      generateContinue(data, (Continue*)statement);
      break;
    case Statement::StatementType::Label:
      generateLabel(data, (Label*)statement);
      break;
    case Statement::StatementType::Goto:
      generateGoto(data, (Goto*)statement);
      break;
    case Statement::StatementType::Declaration:
      generateDeclaration(data, (Declaration*)statement);
      break;
    case Statement::StatementType::IfConditional:
      generateIfConditional(data, (IfConditional*)statement);
      break;
    case Statement::StatementType::SwitchCase:
      generateSwitchCase(data, (SwitchCase*)statement);
      break;
    case Statement::StatementType::SwitchDefault:
      generateSwitchDefault(data, (SwitchDefault*)statement);
      break;
    case Statement::StatementType::SwitchConditional:
      generateSwitchConditional(data, (SwitchConditional*)statement);
      break;
    case Statement::StatementType::DoWhileLoop:
      generateDoWhileLoop(data, (DoWhileLoop*)statement);
      break;
    case Statement::StatementType::WhileLoop:
      generateWhileLoop(data, (WhileLoop*)statement);
      break;
    case Statement::StatementType::ForLoop:
      generateForLoop(data, (ForLoop*)statement);
      break;
  }
}

void GenerateIR::generateCompoundStatement(CommonIRData& data, const CompoundStatement* compoundStatement)
{
  // this check allows statements like for loops to impose the scope prematurely without causing problems
  scopes.emplace_back(compoundStatement);
  for (const std::unique_ptr<Statement>& statement : compoundStatement->body)
  {
    generateStatement(data, statement.get());
  }
  scopes.pop_back();
}

// returns the variable name containing the result of the expression
std::pair<std::string, Operation::DataType> GenerateIR::generateExpression(CommonIRData& data, const Expression* expression)
{
  switch (expression->expressionType)
  {
    case Expression::ExpressionType::Null:
      return {std::to_string((uintptr_t)expression) + "_NullExpression", {}};   
    case Expression::ExpressionType::Constant:
      return generateConstant(data, (Constant*)expression);
    case Expression::ExpressionType::VariableAccess:
      return generateVariableAccess(data, (VariableAccess*)expression);
    case Expression::ExpressionType::FunctionCall:
      return generateFunctionCall(data, (FunctionCall*)expression);
    case Expression::ExpressionType::SubExpression:
      return generateExpression(data, ((SubExpression*)expression)->expression.get());
    case Expression::ExpressionType::PreUnaryOperator:
      return generatePreUnaryOperator(data, (PreUnaryOperator*)expression);
    case Expression::ExpressionType::PostUnaryOperator:
      return generatePostUnaryOperator(data, (PostUnaryOperator*)expression);
    case Expression::ExpressionType::BinaryOperator:
      return generateBinaryOperator(data, (BinaryOperator*)expression);
    case Expression::ExpressionType::TernaryOperator:
      return generateTernaryOperator(data, (TernaryOperator*)expression);
  }
}

std::pair<std::string, Operation::DataType> GenerateIR::generateConstant(CommonIRData& data, const Constant* constant)
{
  // number comes first to ensure that the variable name is unique (identifiers can't start with numbers in C)
  if (constant->dataType->generalType == DataType::GeneralType::PrimitiveType)
  {
    data.irProgram.emplace_back(Operation{ASTTypeToIRType(data, constant->dataType.get()), Operation::Set, {std::to_string((uintptr_t)constant) + "_Constant", std::to_string(*((long int*)constant->value))}});
  }
  
  return {data.irProgram.back().operands[0], data.irProgram.back().type};
}

void GenerateIR::generateReturn(CommonIRData& data, const Return* returnVal)
{
  std::pair<std::string, Operation::DataType> name = generateExpression(data, returnVal->data.get());

  data.irProgram.emplace_back(Operation{name.second, Operation::Return, {name.first}});
}

void GenerateIR::generateBreak(CommonIRData& data, const Break* breakStatement)
{
  bool found = false;
  uint32_t depth = 0;

  for (std::vector<Operation>::const_reverse_iterator op = data.irProgram.rbegin(); op != data.irProgram.rend(); op++)
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
          data.irProgram.emplace_back(Operation{{}, Operation::Jump, {op->operands[0].substr(0, op->operands[0].size()-5) + "End"}});
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

void GenerateIR::generateContinue(CommonIRData& data, const Continue* continueStatement)
{
  bool found = false;
  uint32_t depth = 0;

  for (std::vector<Operation>::const_reverse_iterator op = data.irProgram.rbegin(); op != data.irProgram.rend(); op++)
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
          data.irProgram.emplace_back(Operation{{}, Operation::Jump, {op->operands[0]}});
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

void GenerateIR::generateLabel(CommonIRData& data, const Label* label)
{
  data.irProgram.emplace_back(Operation{{}, Operation::Label, {label->name}});
}

void GenerateIR::generateGoto(CommonIRData& data, const Goto* gotoStatement)
{
  data.irProgram.emplace_back(Operation{{}, Operation::Jump, {gotoStatement->label}});
}

void GenerateIR::generateDeclaration(CommonIRData& data, const Declaration* declaration, bool allowInitialization)
{
  if (declaration->dataType->generalType == DataType::GeneralType::Struct && !((Struct*)declaration->dataType.get())->members.empty())
  {
    uint8_t offset = 0;
    for (std::pair<std::unique_ptr<Declaration>, uint8_t>& m: ((Struct*)declaration->dataType.get())->members)
    {
      Declaration* member = m.first.get();

      Operation::DataType memberType;
      if (member->dataType->generalType != DataType::GeneralType::Pointer)
      {
        memberType = ASTTypeToIRType(data, member->dataType.get());
      } else
      {
        memberType = {data.pointerSize, data.pointerSize};
      }

      uint8_t bitfield = m.second;

      offset += memberType.alignment - offset%memberType.alignment;

      memberOffsets[((Struct*)declaration->dataType.get())->identifier][member->identifier] = {memberType, offset};

      offset += memberType.size;
    }
  }

  if (!declaration->identifier.empty())
  {
    std::string name = declaration->identifier;
    if (!scopes.empty())
    {
      name = "Scope_" + std::to_string((std::uintptr_t)scopes.back()) + "_" + name;
    }
    
    if (declaration->value && declarations.contains(name) && declarations[name]->value)
    {
      std::cout << "IR generation error: Identifier \"" << declaration->identifier << "\" is already defined\n";
      throw IRGenError();
    }

    //declarations[name].reset(copyDataType(variableDeclaration->dataType.get()));
    declarations[name] = declaration;

    if (declaration->value)
    {
      if (!allowInitialization)
      {
        std::cout << "IR generation error: Identifier \"" << declaration->identifier << "\" cannot be initialized here\n";
        throw IRGenError();
      }

      if (declaration->value->statementType == Statement::StatementType::Expression)
      {
        std::pair<std::string, Operation::DataType> outputName = generateExpression(data, (Expression*)declaration->value.get());
        data.irProgram.emplace_back(Operation{outputName.second, Operation::Set, {name, outputName.first}});
      } else 
      {
        scopes.emplace_back((CompoundStatement*)declaration->value.get());

        for (const std::unique_ptr<Declaration>& parameter: ((Function*)declaration->dataType.get())->parameters)
        {
          generateDeclaration(data, parameter.get(), false);
        }

        data.irProgram.emplace_back(Operation{{}, Operation::Label, {name}});
        generateStatement(data, declaration->value.get());
        data.irProgram.emplace_back(Operation{{}, Operation::Return});

        scopes.pop_back();
      }
    }
  }
}

void GenerateIR::generateIfConditional(CommonIRData& data, const IfConditional* ifConditional)
{
  scopes.emplace_back((CompoundStatement*)ifConditional);
  
  // conditional jump to end of if conditional
  std::pair<std::string, Operation::DataType> name = generateExpression(data, ifConditional->condition.get());

  data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfZero, {std::to_string((std::uintptr_t)ifConditional) + "_IfConditionalEnd", name.first}});

  // if conditional body
  generateStatement(data, ifConditional->body.get());

  if (ifConditional->elseStatement)
  {
    // unconditional jump to end of else conditional
    data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)ifConditional) + "_ElseConditionalEnd"}});
  }

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)ifConditional) + "_IfConditionalEnd"}});
  
  if (ifConditional->elseStatement)
  {
    // else body
    generateStatement(data, ifConditional->elseStatement.get());

    data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)ifConditional) + "_ElseConditionalEnd"}});
  }

  scopes.pop_back();
}

void GenerateIR::generateSwitchCase(CommonIRData& data, const SwitchCase* switchCase)
{
  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)switchCase) + "_SwitchCase"}});
}

void GenerateIR::generateSwitchDefault(CommonIRData& data, const SwitchDefault* switchDefault)
{
  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)switchDefault) + "_SwitchDefault"}});
}

void GenerateIR::generateSwitchConditional(CommonIRData& data, const SwitchConditional* switchConditional)
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
  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalBegin"}});

  std::pair<std::string, Operation::DataType> name = generateExpression(data, switchConditional->value.get());

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
          std::pair<std::string, Operation::DataType> caseName = generateExpression(data, ((SwitchCase*)statement)->requirement.get());

          data.irProgram.emplace_back(Operation{name.second, Operation::SetSubtraction, {std::to_string((std::uintptr_t)statement) + "_Subtracted", name.first, caseName.first}});
          data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfZero, {std::to_string((std::uintptr_t)statement) + "_SwitchCase", std::to_string((std::uintptr_t)statement) + "_Subtracted"}});
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
    data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)defaultCase) + "_SwitchDefault"}});
  } else
  {
    data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalEnd"}});
  }

  generateStatement(data, switchConditional->body.get());

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)switchConditional) + "_SwitchConditionalEnd"}});

  scopes.pop_back();
}

void GenerateIR::generateDoWhileLoop(CommonIRData& data, const DoWhileLoop* doWhileLoop)
{
  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopBegin"}});

  generateStatement(data, doWhileLoop->body.get());

  std::pair<std::string, Operation::DataType> name = generateExpression(data, doWhileLoop->condition.get());

  data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopBegin", name.first}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)doWhileLoop) + "_DoWhileLoopEnd"}});
}

void GenerateIR::generateWhileLoop(CommonIRData& data, const WhileLoop* whileLoop)
{
  scopes.emplace_back((CompoundStatement*)whileLoop);

  data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopCondition"}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopBegin"}});

  generateStatement(data, whileLoop->body.get());

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopCondition"}});

  std::pair<std::string, Operation::DataType> name = generateExpression(data, whileLoop->condition.get());

  data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopBegin", name.first}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)whileLoop) + "_WhileLoopEnd"}});

  scopes.pop_back();
}

void GenerateIR::generateForLoop(CommonIRData& data, const ForLoop* forLoop)
{
  scopes.emplace_back((CompoundStatement*)forLoop);
  if (forLoop->initialization)
  {
    generateStatement(data, forLoop->initialization.get());
  }

  data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopCondition"}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin"}});

  generateStatement(data, forLoop->body.get());

  if (forLoop->update)
  {
    generateStatement(data, forLoop->update.get());
  }

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopCondition"}});

  if (forLoop->condition) // condition defaults to constant 1 if not specified
  {
    std::pair<std::string, Operation::DataType> name = generateExpression(data, forLoop->condition.get());

    data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfNotZero, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin", name.first}});
  } else
  {
    data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopBegin"}});
  }

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)forLoop) + "_ForLoopEnd"}});

  scopes.pop_back();
}

std::pair<std::string, Operation::DataType> GenerateIR::generateVariableAccess(CommonIRData& data, const VariableAccess* variableAccess)
{
  std::map<std::string, const Declaration*>::iterator name = getIdentifier(variableAccess->identifier);
  if (name->second)
  {
    return {name->first, ASTTypeToIRType(data, name->second->dataType.get())};
  } else
  {
    std::cout << "IR generation error: \"" << variableAccess->identifier << "\" is not a variable\n";
    throw IRGenError();
  }
}

std::pair<std::string, Operation::DataType> GenerateIR::generateFunctionCall(CommonIRData& data, const FunctionCall* functionCall)
{
  if (declarations.contains(functionCall->identifier) && getIdentifier(functionCall->identifier)->second)
  {
    if (functionCall->arguments.size() != ((Function*)declarations[functionCall->identifier]->dataType.get())->parameters.size())
    {
      std::cout << "IR generation error: Wrong argument count for function \"" << functionCall->identifier << "\". Expected " << ((Function*)declarations[functionCall->identifier]->dataType.get())->parameters.size() << ", received " << functionCall->arguments.size() << "\n";
      throw IRGenError();
    }

    for (const std::unique_ptr<Expression>& argument: functionCall->arguments)
    {
      std::pair<std::string, Operation::DataType> name = generateExpression(data, argument.get());
      data.irProgram.emplace_back(Operation{name.second, Operation::AddArg, {name.first}});
    }

    Operation::DataType returnType = ASTTypeToIRType(data, ((Function*)declarations[functionCall->identifier]->dataType.get())->returnType.get());

    data.irProgram.emplace_back(Operation{returnType, Operation::Call, {std::to_string((uintptr_t)functionCall) + "_ReturnValue", functionCall->identifier}});
    return {std::to_string((uintptr_t)functionCall) + "_ReturnValue", returnType};
  } else
  {
    std::cout << "IR generation error: Function \"" << functionCall->identifier << "\" is not defined\n";
    throw IRGenError();
  }
}

std::pair<std::string, Operation::DataType> GenerateIR::generatePreUnaryOperator(CommonIRData& data, const PreUnaryOperator* preUnary)
{
  std::pair<std::string, Operation::DataType> name = generateExpression(data, preUnary->operand.get());

  switch (preUnary->preUnaryType)
  {
    case PreUnaryOperator::PreUnaryType::Address:
      if (declarations.contains(name.first))
      {
        name.second.pointerDepth++;
        data.irProgram.emplace_back(Operation{name.second, Operation::GetAddress, {std::to_string((uintptr_t)preUnary) + "_Address", name.first}});
      } else
      {
        std::cout << "IR generation error: Getting address of rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case PreUnaryOperator::PreUnaryType::Dereference:
      if (name.second.pointerDepth > 0)
      {
        name.second.pointerDepth--;
        data.irProgram.emplace_back(Operation{name.second, Operation::Dereference, {std::to_string((uintptr_t)preUnary) + "_Dereference", name.first}});
      } else
      {
        std::cout << "IR generation error: Cannot dereference non-pointer type\n";
        throw IRGenError();
      }
      break;
    case PreUnaryOperator::PreUnaryType::TypeCast: {
      TypeCast* typeCast = (TypeCast*)preUnary;
      data.irProgram.emplace_back(Operation{ASTTypeToIRType(data, typeCast->dataType.get()), Operation::Set, {std::to_string((uintptr_t)preUnary) + (typeCast->dataType->isVolatile ? "_Volatile" : "") + "_Casted", name.first}});
      break;
    } case PreUnaryOperator::PreUnaryType::MathematicNegate:
      data.irProgram.emplace_back(Operation{name.second, Operation::Negate, {std::to_string((uintptr_t)preUnary) + "_Negated", name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::BitwiseNOT:
      data.irProgram.emplace_back(Operation{name.second, Operation::BitwiseNOT, {std::to_string((uintptr_t)preUnary) + "_BitwiseNegated", name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::LogicalNegate:
      data.irProgram.emplace_back(Operation{name.second, Operation::LogicalNOT, {std::to_string((uintptr_t)preUnary) + "_LogicalNegated", name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::Increment: {
      if (declarations.contains(name.first))
      {
        data.irProgram.emplace_back(Operation{name.second, Operation::SetAddition, {name.first, name.first, "1"}});
        data.irProgram.emplace_back(Operation{name.second, Operation::Set, {std::to_string((uintptr_t)preUnary) + "_Incremented", name.first}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PreUnaryOperator::PreUnaryType::Decrement:
      if (declarations.contains(name.first))
      {
        data.irProgram.emplace_back(Operation{name.second, Operation::SetSubtraction, {name.first, name.first, "1"}});
        data.irProgram.emplace_back(Operation{name.second, Operation::Set, {std::to_string((uintptr_t)preUnary) + "_Decremented", name.first}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return {data.irProgram.back().operands[0], data.irProgram.back().type};
}

std::pair<std::string, Operation::DataType> GenerateIR::generatePostUnaryOperator(CommonIRData& data, const PostUnaryOperator* postUnary)
{
  std::pair<std::string, Operation::DataType> name = generateExpression(data, postUnary->operand.get());

  switch (postUnary->postUnaryType)
  {
    case PostUnaryOperator::PostUnaryType::Increment: {
      if (declarations.contains(name.first))
      {
        data.irProgram.emplace_back(Operation{name.second, Operation::Set, {std::to_string((uintptr_t)postUnary) + "_PostIncremented", name.first}});
        data.irProgram.emplace_back(Operation{name.second, Operation::SetAddition, {name.first, name.first, "1"}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PostUnaryOperator::PostUnaryType::Decrement:
      if (declarations.contains(name.first))
      {
        data.irProgram.emplace_back(Operation{name.second, Operation::Set, {std::to_string((uintptr_t)postUnary) + "_PostDecremented", name.first}});
        data.irProgram.emplace_back(Operation{name.second, Operation::SetSubtraction, {name.first, name.first, "1"}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return {data.irProgram.end()[-2].operands[0], data.irProgram.end()[-2].type};
}

std::pair<std::string, Operation::DataType> GenerateIR::generateBinaryOperator(CommonIRData& data, const BinaryOperator* binary)
{
  std::pair<std::string, Operation::DataType> leftOperandName = generateExpression(data, binary->leftOperand.get());
  std::pair<std::string, Operation::DataType> rightOperandName;

  if (binary->binaryType != BinaryOperator::BinaryType::DereferenceMemberAccess)
  {
    rightOperandName = generateExpression(data, binary->rightOperand.get());
  } else if (binary->rightOperand->expressionType == Expression::ExpressionType::VariableAccess)
  {
    VariableAccess* identifier = ((VariableAccess*)binary->rightOperand.get());
    std::pair<Operation::DataType, uint8_t>& memberData = memberOffsets[leftOperandName.second.identifier][identifier->identifier];

    data.irProgram.emplace_back(Operation{{1, 1, 1}, Operation::Set, {std::to_string((uintptr_t)binary) + "_Casted", leftOperandName.first}});
    data.irProgram.emplace_back(Operation{memberData.first, Operation::SetAddition, {std::to_string((uintptr_t)binary) + "_Member", std::to_string((uintptr_t)binary) + "_Casted", std::to_string(memberData.second)}});
  }

  switch (binary->binaryType)
  {
    case BinaryOperator::BinaryType::VariableAssignment:
      if (declarations.contains(leftOperandName.first) || leftOperandName.first.find("_Member") != std::string::npos || leftOperandName.first.find("_Dereference") != std::string::npos)
      {
        data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::Set, {leftOperandName.first, rightOperandName.first}});
        data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::Set, {std::to_string((uintptr_t)binary) + "_Assigned", leftOperandName.first}});
      } else
      {
        std::cout << "IR generation error: Assigning rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case BinaryOperator::BinaryType::Add:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetAddition, {std::to_string((uintptr_t)binary) + "_Added", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Subtract:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetSubtraction, {std::to_string((uintptr_t)binary) + "_Subtracted", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Multiply:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetMultiplication, {std::to_string((uintptr_t)binary) + "_Multiplied", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Divide:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetDivision, {std::to_string((uintptr_t)binary) + "_Divided", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Modulo:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetModulo, {std::to_string((uintptr_t)binary) + "_Moduloed", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LeftShift:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetLeftShift, {std::to_string((uintptr_t)binary) + "_LeftShifted", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::RightShift:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetRightShift, {std::to_string((uintptr_t)binary) + "_RightShifted", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseOR:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseOR, {std::to_string((uintptr_t)binary) + "_BitwiseOred", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseAND:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseAND, {std::to_string((uintptr_t)binary) + "_BitwiseAnded", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseXOR:
      data.irProgram.emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseXOR, {std::to_string((uintptr_t)binary) + "_BitwiseXored", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LogicalOR:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetLogicalOR, {std::to_string((uintptr_t)binary) + "_LogicalOred", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LogicalAND:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetLogicalAND, {std::to_string((uintptr_t)binary) + "_LogicalAnded", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Subscript:

      break;
    case BinaryOperator::BinaryType::Equal:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetEqual, {std::to_string((uintptr_t)binary) + "_Equaled", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::NotEqual:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetNotEqual, {std::to_string((uintptr_t)binary) + "_NotEqualed", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Greater:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetGreater, {std::to_string((uintptr_t)binary) + "_Large", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Lesser:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetLesser, {std::to_string((uintptr_t)binary) + "_Small", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::GreaterOrEqual:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetGreaterOrEqual, {std::to_string((uintptr_t)binary) + "_Largeish", leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LesserOrEqual:
      data.irProgram.emplace_back(Operation{{1, 1}, Operation::SetLesserOrEqual, {std::to_string((uintptr_t)binary) + "_Smallish", leftOperandName.first, rightOperandName.first}});
      break;
  }

  return {data.irProgram.back().operands[0], data.irProgram.back().type};
}

std::pair<std::string, Operation::DataType> GenerateIR::generateTernaryOperator(CommonIRData& data, const TernaryOperator* ternary)
{
  // conditional jump to end of if conditional
  std::pair<std::string, Operation::DataType> name = generateExpression(data, ternary->condition.get());

  data.irProgram.emplace_back(Operation{name.second, Operation::JumpIfZero, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorFalse", name.first}});

  // if conditional body
  std::pair<std::string, Operation::DataType> trueName = generateExpression(data, ternary->trueOperand.get());
  data.irProgram.emplace_back(Operation{trueName.second, Operation::Set, {name.first, trueName.first}});

  // unconditional jump to end of else conditional
  data.irProgram.emplace_back(Operation{{}, Operation::Jump, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorEnd"}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorFalse"}});

  // else body
  std::pair<std::string, Operation::DataType> falseName = generateExpression(data, ternary->falseOperand.get());
  data.irProgram.emplace_back(Operation{falseName.second, Operation::Set, {name.first, falseName.first}});

  data.irProgram.emplace_back(Operation{{}, Operation::Label, {std::to_string((std::uintptr_t)ternary) + "_TernaryOperatorEnd"}});

  return name;
}



bool GenerateIR::resolveConstantOperations(std::vector<Operation> &irProgram)
{
  bool changed = false;

  std::map<std::string, std::string> vars;

  for (std::vector<Operation>::iterator i = irProgram.begin(); i != irProgram.end(); i++)
  {
    switch (i->code)
    {
      case Operation::Set:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = i->operands[1];
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        }
        break;
      case Operation::Return:
        if (vars.contains(i->operands[0]))
        {
          i->operands[0] = vars[i->operands[0]];
          changed = true;
        }
        break;
      case Operation::Dereference:
      case Operation::SetAddition:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos && i->operands[2].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) + std::stoi(i->operands[2]));
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        } else if (vars.contains(i->operands[2]))
        {
          i->operands[2] = vars[i->operands[2]];
          changed = true;
        }
        break;
      case Operation::SetSubtraction:
      case Operation::SetMultiplication:
      case Operation::SetDivision:
      case Operation::SetModulo:
      case Operation::SetBitwiseAND:
      case Operation::SetBitwiseOR:
      case Operation::SetBitwiseXOR:
      case Operation::SetLeftShift:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos && i->operands[2].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) << std::stoi(i->operands[2]));
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        } else if (vars.contains(i->operands[2]))
        {
          i->operands[2] = vars[i->operands[2]];
          changed = true;
        }
        break;
      case Operation::SetRightShift:
      case Operation::SetLogicalAND:
      case Operation::SetLogicalOR:
      case Operation::SetEqual:
      case Operation::SetNotEqual:
      case Operation::SetGreater:
      case Operation::SetLesser:
      case Operation::SetGreaterOrEqual:
      case Operation::SetLesserOrEqual:
      case Operation::Negate:
      case Operation::LogicalNOT:
      case Operation::BitwiseNOT:
        vars.erase(i->operands[0]);
        break;
      case Operation::Label:
        vars.clear();
        break;
    }
  }

  return changed;
}

bool GenerateIR::trimInaccessibleCode(std::vector<Operation> &irProgram)
{
  bool changed = false;
  bool accessible = true;

  std::map<std::string, uint8_t> identifiers;

  for (std::vector<Operation>::iterator i = irProgram.begin(); i != irProgram.end(); i++)
  {
    if (!accessible)
    {
      irProgram.erase(i--);
      changed = true;
    }

    if (i->code == Operation::Label)
    {
      identifiers[i->operands[0]] |= 2;
      accessible = true;
    } else if (i->code == Operation::Jump || i->code == Operation::Return)
    {
      accessible = false;
    }

    if (i->code == Operation::Jump || i->code == Operation::JumpIfZero || i->code == Operation::JumpIfNotZero || i->code == Operation::Call)
    {
      identifiers[i->operands[0]] |= 1;
    }

    if (i->code == Operation::Set || 
      i->code == Operation::Dereference || 
      i->code == Operation::SetAddition || 
      i->code == Operation::SetSubtraction || 
      i->code == Operation::SetMultiplication || 
      i->code == Operation::SetDivision || 
      i->code == Operation::SetModulo || 
      i->code == Operation::SetBitwiseAND || 
      i->code == Operation::SetBitwiseOR || 
      i->code == Operation::SetBitwiseXOR || 
      i->code == Operation::SetLeftShift || 
      i->code == Operation::SetRightShift || 
      i->code == Operation::SetLogicalAND || 
      i->code == Operation::SetLogicalOR || 
      i->code == Operation::SetEqual || 
      i->code == Operation::SetNotEqual || 
      i->code == Operation::SetGreater || 
      i->code == Operation::SetLesser || 
      i->code == Operation::SetGreaterOrEqual || 
      i->code == Operation::SetLesserOrEqual || 
      i->code == Operation::Negate || 
      i->code == Operation::LogicalNOT || 
      i->code == Operation::BitwiseNOT)
    {
      if (i->operands[0].find("_Volatile") == std::string::npos)
      {
        identifiers[i->operands[0]] |= 2;
      }
      if (!i->operands[1].empty() && i->operands[1].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[1]] |= 1;
      }
      if (!i->operands[2].empty() && i->operands[2].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[2]] |= 1;
      }
    } else if (i->code != Operation::Label && 
      i->code != Operation::Jump)
    {
      if (!i->operands[0].empty() && i->operands[0].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[0]] |= 1;
      }
      if (!i->operands[1].empty() && i->operands[1].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[1]] |= 1;
      }
      if (!i->operands[2].empty() && i->operands[2].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[2]] |= 1;
      }
    }
  }

  while (!identifiers.empty() && (identifiers.begin()->second != 2 || identifiers.begin()->first == "main_Function"))
  {
    identifiers.erase(identifiers.begin());
  }

  for (std::map<std::string, uint8_t>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
  {
    if (l->second != 2 || l->first == "main_Function")
    {
      identifiers.erase(l--);
    }
  }

  if (identifiers.empty())
  {
    return changed;
  }

  for (std::vector<Operation>::iterator i = irProgram.begin(); i != irProgram.end(); i++)
  {
    if (i->code == Operation::Set || 
      i->code == Operation::Dereference || 
      i->code == Operation::SetAddition || 
      i->code == Operation::SetSubtraction || 
      i->code == Operation::SetMultiplication || 
      i->code == Operation::SetDivision || 
      i->code == Operation::SetModulo || 
      i->code == Operation::SetBitwiseAND || 
      i->code == Operation::SetBitwiseOR || 
      i->code == Operation::SetBitwiseXOR || 
      i->code == Operation::SetLeftShift || 
      i->code == Operation::SetRightShift || 
      i->code == Operation::SetLogicalAND || 
      i->code == Operation::SetLogicalOR || 
      i->code == Operation::SetEqual || 
      i->code == Operation::SetNotEqual || 
      i->code == Operation::SetGreater || 
      i->code == Operation::SetLesser || 
      i->code == Operation::SetGreaterOrEqual || 
      i->code == Operation::SetLesserOrEqual || 
      i->code == Operation::Negate || 
      i->code == Operation::LogicalNOT || 
      i->code == Operation::BitwiseNOT || 
      i->code == Operation::Label)
    {
      for (std::map<std::string, uint8_t>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
      {
        if (i->operands[0] == l->first)
        {
          irProgram.erase(i--);
          changed = true;
        }
      }
    }
  }

  return changed;
}

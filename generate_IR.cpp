#include "generate_IR.hpp"

#include <cassert>
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
      *((PrimitiveType*)result) = *((PrimitiveType*)data;
      break;
    case DataType::GeneralType::Pointer:
      result = new Pointer;
      ((Pointer*)result)->dataType.reset(copyDataType(((Pointer*)data)->dataType));
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

IRprogram GenerateIR::generateIR(const Program& AST, const Compiler::TypeSizes& typeSizes)
{
  CommonIRData data{
    nullptr,
    IRprogram{
      std::vector<uint8_t>{},
      std::map<std::string, std::size_t>{},
      std::vector<IRprogram::Function>{
        IRprogram::Function{{}, {}}
      }
    },
    typeSizes
  };
  data.instrArray = &data.irProgram.program[0].body;

  dynamicData.first.reset(new uint8_t[65535]);
  uid.reset();

  for (const ASTnode* node: AST.nodes)
  {
    if (node->nodeType == ASTnode::NodeType::Statement)
    {
      if (((Statement*)node)->statementType == Statement::StatementType::Declaration)
      {
        generateDeclaration(data, (Declaration*)node);
      }
    }
  }

  return data.irProgram;
}

void GenerateIR::optimizeIR(IRprogram& irProgram)
{
  bool changed;

  do
  {
    changed = false;

    changed |= resolveConstantOperations(irProgram);
    changed |= trimInaccessibleCode(irProgram);
  } while (changed);
}

PrimitiveType GenerateIR::ASTTypeToIRType(CommonIRData& data, const DataType* dataType)
{
  switch (dataType->generalType)
  {
    case DataType::GeneralType::PrimitiveType: {
      PrimitiveType* primitiveType = (PrimitiveType*)dataType;

      return {primitiveType->size, primitiveType->alignment, primitiveType->isFloating, primitiveType->isSigned, dataType->isConst, dataType->isVolatile};
    } case DataType::GeneralType::Pointer: {
      Pointer* pointer = (Pointer*)dataType;
      
      return {data.typeSizes.pointerSize, data.typeSizes.pointerSize, false, false, dataType->isConst, dataType->isVolatile};
    } case DataType::GeneralType::Array: {
      Array* array = (Array*)dataType;

      // We store the size of each element in .size, and the size of the array in elements in .arrayLength
      PrimitiveType childType = ASTTypeToIRType(data, array->dataType);

      if (array->size->expressionType == Expression::ExpressionType::Constant)
      {
        childType.size *= *(uint64_t*)(((Constant*)array->size)->value);
      }
      
      return childType;
    } case DataType::GeneralType::Struct: {
      Struct* structure = (Struct*)dataType;
      
      PrimitiveType result;
      result.isConst = dataType->isConst;
      result.isVolatile = dataType->isVolatile;

      if (memberOffsets.contains(std::string(structure->identifier)))
      {
        for (const std::pair<std::string, std::pair<PrimitiveType, uint8_t>>& member: memberOffsets[std::string(structure->identifier)].second)
        {
          result.alignment = std::max(result.alignment, member.second.first.alignment);
          
          if (result.size%member.second.first.alignment != 0)
          {
            result.size += member.second.first.alignment - result.size%member.second.first.alignment;
          }
          result.size += member.second.first.size;
        }
      } else
      {
        for (const std::pair<Declaration*, uint8_t>& member: structure->members)
        {
          PrimitiveType memberType;
          if (member.first->dataType->generalType != DataType::GeneralType::Pointer)
          {
            memberType = ASTTypeToIRType(data, member.first->dataType);
          } else
          {
            memberType = {data.typeSizes.pointerSize, data.typeSizes.pointerSize};
          }

          result.alignment = std::max(result.alignment, memberType.alignment);
          
          if (result.size%memberType.alignment != 0)
          {
            result.size += memberType.alignment - result.size%memberType.alignment;
          }
          result.size += memberType.size;
        }
      }

      return result;
    } case DataType::GeneralType::Function:

      return {};
  }
}

std::map<std::string, DataType*>::iterator GenerateIR::getIdentifier(const std::string& identifier)
{
  std::map<std::string, DataType*>::iterator parentVar = declarations.end();
  for (std::vector<const CompoundStatement*>::const_reverse_iterator scope = scopes.crbegin(); scope != scopes.crend(); scope++)
  {
    if (declarations.contains(uid.get("Scope_", *scope) + "_" + identifier))
    {
      parentVar = declarations.find(uid.get("Scope_", *scope) + "_" + identifier);
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
  for (const Statement* statement: compoundStatement->body)
  {
    generateStatement(data, statement);
  }
  scopes.pop_back();
}

// returns the variable name containing the result of the expression
std::pair<std::string, PrimitiveType> GenerateIR::generateExpression(CommonIRData& data, const Expression* expression, bool decayArraysToPointers)
{
  std::pair<std::string, PrimitiveType> result;

  switch (expression->expressionType)
  {
    case Expression::ExpressionType::Null:
      result = {uid.get("_NullExpression", expression), {}};   
      break;
    case Expression::ExpressionType::Constant:
      result = generateConstant(data, (Constant*)expression);
      break;
    case Expression::ExpressionType::VariableAccess:
      result = generateVariableAccess(data, (VariableAccess*)expression);
      break;
    case Expression::ExpressionType::StringLiteral:
      result = generateStringLiteral(data, (StringLiteral*)expression);
      break;
    case Expression::ExpressionType::FunctionCall:
      result = generateFunctionCall(data, (FunctionCall*)expression);
      break;
    case Expression::ExpressionType::SubExpression:
      result = generateExpression(data, ((SubExpression*)expression)->expression);
      break;
    case Expression::ExpressionType::PreUnaryOperator:
      result = generatePreUnaryOperator(data, (PreUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::PostUnaryOperator:
      result = generatePostUnaryOperator(data, (PostUnaryOperator*)expression);
      break;
    case Expression::ExpressionType::BinaryOperator:
      result = generateBinaryOperator(data, (BinaryOperator*)expression);
      break;
    case Expression::ExpressionType::TernaryOperator:
      result = generateTernaryOperator(data, (TernaryOperator*)expression);
      break;
  }

  // handle array to pointer decay
  if (decayArraysToPointers && declarations.contains(result.first) && declarations[result.first]->generalType == DataType::GeneralType::Array)
  {
    Pointer* addressType;
    addressType = arenaAlloc(addressType);
    addressType->dataType = ((Array*)declarations[result.first])->dataType;
    declarations[uid.get("_Decayed", expression)] = addressType;

    data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::GetAddress, {uid.get("_Decayed", expression), result.first}});

    result = {data.instrArray->back().operands[0], data.instrArray->back().type};
  }

  return result;
}

std::pair<std::string, PrimitiveType> GenerateIR::generateConstant(CommonIRData& data, Constant* constant)
{
  declarations[uid.get("_Constant", constant)] = &constant->dataType;

  // number comes first to ensure that the variable name is unique (identifiers can't start with numbers in C)
  data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, &constant->dataType), Operation::Set, {uid.get("_Constant", constant), "NaN"}});

  if (constant->dataType.isFloating)
  {
    if (constant->dataType.size == data.typeSizes.floatSize)
    {
      data.instrArray->back().operands[1] = std::to_string(*((float*)constant->value));
    } else if (constant->dataType.size == data.typeSizes.doubleSize)
    {
      data.instrArray->back().operands[1] = std::to_string(*((double*)constant->value));
    } else if (constant->dataType.size == data.typeSizes.longDoubleSize)
    {
      data.instrArray->back().operands[1] = std::to_string(*((long double*)constant->value));
    }
  } else if (constant->dataType.isSigned)
  {
    data.instrArray->back().operands[1] = std::to_string(*((int64_t*)constant->value));
  } else
  {
    data.instrArray->back().operands[1] = std::to_string(*((uint64_t*)constant->value));
  }
  
  return {data.instrArray->back().operands[0], data.instrArray->back().type};
}

void GenerateIR::generateReturn(CommonIRData& data, const Return* returnVal)
{
  std::pair<std::string, PrimitiveType> name = generateExpression(data, returnVal->data);

  data.instrArray->emplace_back(Operation{name.second, Operation::Return, {name.first}});
}

void GenerateIR::generateBreak(CommonIRData& data, const Break* breakStatement)
{
  bool found = false;
  uint32_t depth = 0;

  for (std::vector<Operation>::const_reverse_iterator op = data.instrArray->rbegin(); op != data.instrArray->rend(); op++)
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
          data.instrArray->emplace_back(Operation{{}, Operation::Jump, {op->operands[0].substr(0, op->operands[0].size()-5) + "End"}});
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

  for (std::vector<Operation>::const_reverse_iterator op = data.instrArray->rbegin(); op != data.instrArray->rend(); op++)
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
          data.instrArray->emplace_back(Operation{{}, Operation::Jump, {op->operands[0]}});
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
  data.instrArray->emplace_back(Operation{{}, Operation::Label, {std::string(label->name)}});
}

void GenerateIR::generateGoto(CommonIRData& data, const Goto* gotoStatement)
{
  data.instrArray->emplace_back(Operation{{}, Operation::Jump, {std::string(gotoStatement->label)}});
}

void GenerateIR::generateDeclaration(CommonIRData& data, const Declaration* declaration, bool allowInitialization)
{
  PrimitiveType declarationType = ASTTypeToIRType(data, declaration->dataType);

  if (declaration->dataType->generalType == DataType::GeneralType::Struct && !((Struct*)declaration->dataType)->members.empty())
  {
    memberOffsets[std::string(((Struct*)declaration->dataType)->identifier)].first = declaration->dataType;

    uint8_t offset = 0;
    for (std::pair<Declaration*, uint8_t>& m: ((Struct*)declaration->dataType)->members)
    {
      Declaration* member = m.first;

      PrimitiveType memberType = ASTTypeToIRType(data, member->dataType);

      uint8_t bitfield = m.second;

      if (offset%memberType.alignment != 0)
      {
        offset += memberType.alignment - offset%memberType.alignment;
      }

      memberOffsets[std::string(((Struct*)declaration->dataType)->identifier)].second[std::string(member->identifier)] = {ASTTypeToIRType(data, member->dataType), offset};

      offset += memberType.size;
    }
  }

  if (!declaration->identifier.empty())
  {
    std::string name = std::string(declaration->identifier);
    if (!scopes.empty())
    {
      name = uid.get("Scope_", scopes.back()) + "_" + name;
    }
    
    if (declaration->value && declarations.contains(name))
    {
      for (const IRprogram::Function& function: data.irProgram.program)
      {
        if (!function.body.empty() && function.body[0].code == Operation::Label && function.body[0].operands[0] == name)
        {
          std::cout << "IR generation error: Identifier \"" << declaration->identifier << "\" is already defined\n";
          throw IRGenError();
        }
      }
    }

    //declarations[name].reset(copyDataType(variableDeclaration->dataType));
    declarations[name] = declaration->dataType;

    if (declaration->value)
    {
      if (!allowInitialization)
      {
        std::cout << "IR generation error: Identifier \"" << declaration->identifier << "\" cannot be initialized here\n";
        throw IRGenError();
      }

      if (declaration->value->statementType == Statement::StatementType::Expression)
      {
        if (scopes.empty())
        {
          data.irProgram.staticVariables[name] = data.irProgram.staticData.size();
          data.irProgram.staticData.resize(data.irProgram.staticData.size()+declarationType.size);
        }

        if (scopes.empty() && ((Expression*)declaration->value)->expressionType == Expression::ExpressionType::Constant)
        {
          // move the variable value into the static data array
          std::copy(((Constant*)declaration->value)->value, ((Constant*)declaration->value)->value+declarationType.size, data.irProgram.staticData.data()+data.irProgram.staticVariables[name]);
        } else
        {
          std::pair<std::string, PrimitiveType> outputName = generateExpression(data, (Expression*)declaration->value);
          data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, declaration->dataType), Operation::Set, {name, outputName.first}});
        }
      } else 
      {
        scopes.emplace_back((CompoundStatement*)declaration->value);

        // Only create a new function if the last one has something in it. Otherwise, reuse the old one.
        if (!data.instrArray->empty() || !data.irProgram.program.back().parameters.empty())
        {
          data.irProgram.program.emplace_back();
          data.instrArray = &data.irProgram.program.back().body;
        }

        for (const Declaration* parameter: ((Function*)declaration->dataType)->parameters)
        {
          generateDeclaration(data, parameter, false);

          data.irProgram.program.back().parameters.emplace_back(getIdentifier(std::string(parameter->identifier))->first, ASTTypeToIRType(data, parameter->dataType));
        }

        data.instrArray->emplace_back(Operation{{}, Operation::Label, {name}});
        generateStatement(data, declaration->value);
        data.instrArray->emplace_back(Operation{{}, Operation::Return});

        scopes.pop_back();
      }
    }
  }
}

void GenerateIR::generateIfConditional(CommonIRData& data, const IfConditional* ifConditional)
{
  scopes.emplace_back((CompoundStatement*)ifConditional);
  
  // conditional jump to end of if conditional
  std::pair<std::string, PrimitiveType> name = generateExpression(data, ifConditional->condition);

  data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfZero, {uid.get("_IfConditionalEnd", ifConditional), name.first}});

  // if conditional body
  generateStatement(data, ifConditional->body);

  if (ifConditional->elseStatement)
  {
    // unconditional jump to end of else conditional
    data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_ElseConditionalEnd", ifConditional)}});
  }

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_IfConditionalEnd", ifConditional)}});
  
  if (ifConditional->elseStatement)
  {
    // else body
    generateStatement(data, ifConditional->elseStatement);

    data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_ElseConditionalEnd", ifConditional)}});
  }

  scopes.pop_back();
}

void GenerateIR::generateSwitchCase(CommonIRData& data, const SwitchCase* switchCase)
{
  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_SwitchCase", switchCase)}});
}

void GenerateIR::generateSwitchDefault(CommonIRData& data, const SwitchDefault* switchDefault)
{
  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_SwitchDefault", switchDefault)}});
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
  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_SwitchConditionalBegin", switchConditional)}});

  std::pair<std::string, PrimitiveType> name = generateExpression(data, switchConditional->value);

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
          std::pair<std::string, PrimitiveType> caseName = generateExpression(data, ((SwitchCase*)statement)->requirement);

          data.instrArray->emplace_back(Operation{name.second, Operation::SetSubtraction, {uid.get("_Subtracted", statement), name.first, caseName.first}});
          data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfZero, {uid.get("_SwitchCase", statement), uid.get("_Subtracted", statement)}});
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
    data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_SwitchDefault", defaultCase)}});
  } else
  {
    data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_SwitchConditionalEnd", switchConditional)}});
  }

  generateStatement(data, switchConditional->body);

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_SwitchConditionalEnd", switchConditional)}});

  scopes.pop_back();
}

void GenerateIR::generateDoWhileLoop(CommonIRData& data, const DoWhileLoop* doWhileLoop)
{
  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_DoWhileLoopBegin", doWhileLoop)}});

  generateStatement(data, doWhileLoop->body);

  std::pair<std::string, PrimitiveType> name = generateExpression(data, doWhileLoop->condition);

  data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfNotZero, {uid.get("_DoWhileLoopBegin", doWhileLoop), name.first}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_DoWhileLoopEnd", doWhileLoop)}});
}

void GenerateIR::generateWhileLoop(CommonIRData& data, const WhileLoop* whileLoop)
{
  scopes.emplace_back((CompoundStatement*)whileLoop);

  data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_WhileLoopCondition", whileLoop)}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_WhileLoopBegin", whileLoop)}});

  generateStatement(data, whileLoop->body);

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_WhileLoopCondition", whileLoop)}});

  std::pair<std::string, PrimitiveType> name = generateExpression(data, whileLoop->condition);

  data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfNotZero, {uid.get("_WhileLoopBegin", whileLoop), name.first}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_WhileLoopEnd", whileLoop)}});

  scopes.pop_back();
}

void GenerateIR::generateForLoop(CommonIRData& data, const ForLoop* forLoop)
{
  scopes.emplace_back((CompoundStatement*)forLoop);
  if (forLoop->initialization)
  {
    generateStatement(data, forLoop->initialization);
  }

  data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_ForLoopCondition", forLoop)}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_ForLoopBegin", forLoop)}});

  generateStatement(data, forLoop->body);

  if (forLoop->update)
  {
    generateStatement(data, forLoop->update);
  }

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_ForLoopCondition", forLoop)}});

  if (forLoop->condition) // condition defaults to constant 1 if not specified
  {
    std::pair<std::string, PrimitiveType> name = generateExpression(data, forLoop->condition);

    data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfNotZero, {uid.get("_ForLoopBegin", forLoop), name.first}});
  } else
  {
    data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_ForLoopBegin", forLoop)}});
  }

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_ForLoopEnd", forLoop)}});

  scopes.pop_back();
}

std::pair<std::string, PrimitiveType> GenerateIR::generateVariableAccess(CommonIRData& data, const VariableAccess* variableAccess)
{
  std::map<std::string, DataType*>::iterator name = getIdentifier(std::string(variableAccess->identifier));
  if (name->second)
  {
    return {name->first, ASTTypeToIRType(data, name->second)};
  } else
  {
    std::cout << "IR generation error: \"" << variableAccess->identifier << "\" is not a variable\n";
    throw IRGenError();
  }
}

std::pair<std::string, PrimitiveType> GenerateIR::generateStringLiteral(CommonIRData& data, const StringLiteral* stringLiteral)
{
  data.irProgram.staticVariables[uid.get("_Literal", stringLiteral)] = data.irProgram.staticData.size();
  data.irProgram.staticData.insert(data.irProgram.staticData.end(), stringLiteral->value.begin(), stringLiteral->value.end()+1);

  // ensure null termination
  data.irProgram.staticData.back() = 0;

  Array* astType;
  astType = arenaAlloc(astType);

  astType->size = arenaAlloc((Constant*)astType->size);

  ((Constant*)astType->size)->dataType.size = data.typeSizes.pointerSize;
  *(uint16_t*)((Constant*)astType->size)->value = uint16_t(stringLiteral->value.size()+1);

  declarations[uid.get("_Literal", stringLiteral)] = astType;

  return {uid.get("_Literal", stringLiteral), {uint16_t(stringLiteral->value.size()+1), 1, false, true, true, false}};
}

std::pair<std::string, PrimitiveType> GenerateIR::generateFunctionCall(CommonIRData& data, const FunctionCall* functionCall)
{
  if (declarations.contains(std::string(functionCall->identifier)) && getIdentifier(std::string(functionCall->identifier))->second)
  {
    if (functionCall->arguments.size() != ((Function*)declarations[std::string(functionCall->identifier)])->parameters.size())
    {
      std::cout << "IR generation error: Wrong argument count for function \"" << functionCall->identifier << "\". Expected " << ((Function*)declarations[std::string(functionCall->identifier)])->parameters.size() << ", received " << functionCall->arguments.size() << "\n";
      throw IRGenError();
    }

    for (const Expression* argument: functionCall->arguments)
    {
      std::pair<std::string, PrimitiveType> name = generateExpression(data, argument);
      data.instrArray->emplace_back(Operation{name.second, Operation::AddArg, {name.first}});
    }

    declarations[uid.get("_ReturnValue", functionCall)] = ((Function*)declarations[std::string(functionCall->identifier)])->returnType;

    PrimitiveType returnType = ASTTypeToIRType(data, ((Function*)declarations[std::string(functionCall->identifier)])->returnType);

    data.instrArray->emplace_back(Operation{returnType, Operation::Call, {std::string(functionCall->identifier), uid.get("_ReturnValue", functionCall)}});
    return {uid.get("_ReturnValue", functionCall), returnType};
  } else
  {
    std::cout << "IR generation error: Function \"" << functionCall->identifier << "\" is not defined\n";
    throw IRGenError();
  }
}

std::pair<std::string, PrimitiveType> GenerateIR::generatePreUnaryOperator(CommonIRData& data, const PreUnaryOperator* preUnary)
{
  std::pair<std::string, PrimitiveType> name = generateExpression(data, preUnary->operand, preUnary->preUnaryType != PreUnaryOperator::PreUnaryType::Address && preUnary->preUnaryType != PreUnaryOperator::PreUnaryType::Sizeof);

  switch (preUnary->preUnaryType)
  {
    case PreUnaryOperator::PreUnaryType::Address:
      // Intermediate variables start with numbers
      if (name.first[0] < '0' || name.first[0] > '9' || name.first.find("_Dereference") != std::string::npos)
      {
        Pointer* addressType;
        addressType = arenaAlloc(addressType);
        addressType->dataType = declarations[name.first];
        declarations[uid.get("_Address", preUnary)] = addressType;

        if (!data.instrArray->empty() && data.instrArray->back().code == Operation::DereferenceRValue)
        {
          data.instrArray->back().code = Operation::Set;
          data.instrArray->back().type = {data.typeSizes.pointerSize, data.typeSizes.pointerSize};
          data.instrArray->back().operands[0] = uid.get("_Address", preUnary);
        } else
        {
          data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::GetAddress, {uid.get("_Address", preUnary), name.first}});
        }
      } else
      {
        std::cout << "IR generation error: Getting address of rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case PreUnaryOperator::PreUnaryType::Dereference:
      if (declarations[name.first]->generalType == DataType::GeneralType::Pointer ||
        declarations[name.first]->generalType == DataType::GeneralType::Array)
      {
        DataType* resultType;
        if (declarations[name.first]->generalType == DataType::GeneralType::Pointer)
        {
          resultType = ((Pointer*)declarations[name.first])->dataType;
        } else if (declarations[name.first]->generalType == DataType::GeneralType::Array)
        {
          resultType = ((Array*)declarations[name.first])->dataType;
        }

        declarations[uid.get("_Dereference", preUnary)] = resultType;

        data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, resultType), Operation::DereferenceRValue, {uid.get("_Dereference", preUnary), name.first}});
      } else
      {
        std::cout << "IR generation error: Cannot dereference non-pointer type\n";
        throw IRGenError();
      }
      break;
    case PreUnaryOperator::PreUnaryType::Sizeof:
      declarations[uid.get("_Size", preUnary)] = (PrimitiveType*)arenaAlloc((PrimitiveType*)declarations[uid.get("_Size", preUnary)]);
      ((PrimitiveType*)declarations[uid.get("_Size", preUnary)])->size = data.typeSizes.pointerSize;
      ((PrimitiveType*)declarations[uid.get("_Size", preUnary)])->alignment = data.typeSizes.pointerSize;

      data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::Set, {uid.get("_Size", preUnary), std::to_string(name.second.size)}});
      break;
    case PreUnaryOperator::PreUnaryType::TypeCast: {
      TypeCast* typeCast = (TypeCast*)preUnary;
      
      declarations[uid.get("_Casted", preUnary)] = typeCast->dataType;

      data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, typeCast->dataType), Operation::Set, {uid.get("_Casted", preUnary), name.first}});
      break;
    } case PreUnaryOperator::PreUnaryType::MathematicNegate:
      declarations[uid.get("_Negated", preUnary)] = declarations[name.first];

      data.instrArray->emplace_back(Operation{name.second, Operation::Negate, {uid.get("_Negated", preUnary), name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::BitwiseNOT:
      declarations[uid.get("_BitwiseNegated", preUnary)] = declarations[name.first];

      data.instrArray->emplace_back(Operation{name.second, Operation::BitwiseNOT, {uid.get("_BitwiseNegated", preUnary), name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::LogicalNegate:
      declarations[uid.get("_LogicalNegated", preUnary)] = declarations[name.first];

      data.instrArray->emplace_back(Operation{name.second, Operation::LogicalNOT, {uid.get("_LogicalNegated", preUnary), name.first}});
      break;
    case PreUnaryOperator::PreUnaryType::Increment: {
      if (name.first[0] < '0' || name.first[0] > '9')
      {
        declarations[uid.get("_Incremented", preUnary)] = declarations[name.first];

        data.instrArray->emplace_back(Operation{name.second, Operation::SetAddition, {name.first, name.first, "1"}});
        data.instrArray->emplace_back(Operation{name.second, Operation::Set, {uid.get("_Incremented", preUnary), name.first}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PreUnaryOperator::PreUnaryType::Decrement:
      if (name.first[0] < '0' || name.first[0] > '9')
      {
        declarations[uid.get("_Decremented", preUnary)] = declarations[name.first];

        data.instrArray->emplace_back(Operation{name.second, Operation::SetSubtraction, {name.first, name.first, "1"}});
        data.instrArray->emplace_back(Operation{name.second, Operation::Set, {uid.get("_Decremented", preUnary), name.first}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return {data.instrArray->back().operands[0], data.instrArray->back().type};
}

std::pair<std::string, PrimitiveType> GenerateIR::generatePostUnaryOperator(CommonIRData& data, const PostUnaryOperator* postUnary)
{
  std::pair<std::string, PrimitiveType> name = generateExpression(data, postUnary->operand);

  switch (postUnary->postUnaryType)
  {
    case PostUnaryOperator::PostUnaryType::Increment: {
      if (name.first[0] < '0' || name.first[0] > '9')
      {
        declarations[uid.get("_PostIncremented", postUnary)] = declarations[name.first];

        data.instrArray->emplace_back(Operation{name.second, Operation::Set, {uid.get("_PostIncremented", postUnary), name.first}});
        data.instrArray->emplace_back(Operation{name.second, Operation::SetAddition, {name.first, name.first, "1"}});
      } else
      {
        std::cout << "IR generation error: Incrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    } case PostUnaryOperator::PostUnaryType::Decrement:
      if (name.first[0] < '0' || name.first[0] > '9')
      {
        declarations[uid.get("_PostDecremented", postUnary)] = declarations[name.first];

        data.instrArray->emplace_back(Operation{name.second, Operation::Set, {uid.get("_PostDecremented", postUnary), name.first}});
        data.instrArray->emplace_back(Operation{name.second, Operation::SetSubtraction, {name.first, name.first, "1"}});
      } else
      {
        std::cout << "IR generation error: Decrementing rvalue is illegal\n";
        throw IRGenError();
      }
      break;
  }

  return {data.instrArray->end()[-2].operands[0], data.instrArray->end()[-2].type};
}

std::pair<std::string, PrimitiveType> GenerateIR::generateBinaryOperator(CommonIRData& data, const BinaryOperator* binary)
{
  std::pair<std::string, PrimitiveType> leftOperandName;
  std::pair<std::string, PrimitiveType> rightOperandName;

  if (
    binary->binaryType < BinaryOperator::BinaryType::VariableAssignment ||
    binary->binaryType > BinaryOperator::BinaryType::BitwiseXOREqual ||
    ((binary->leftOperand->expressionType != Expression::ExpressionType::PreUnaryOperator ||
    ((PreUnaryOperator*)binary->leftOperand)->preUnaryType != PreUnaryOperator::PreUnaryType::Dereference) &&
    (binary->leftOperand->expressionType != Expression::ExpressionType::BinaryOperator &&
    (((BinaryOperator*)binary->leftOperand)->binaryType != BinaryOperator::BinaryType::Subscript ||
    ((BinaryOperator*)binary->leftOperand)->binaryType != BinaryOperator::BinaryType::DereferenceMemberAccess))))
  {
    leftOperandName = generateExpression(data, binary->leftOperand);
  }

  if (binary->binaryType != BinaryOperator::BinaryType::DereferenceMemberAccess)
  {
    rightOperandName = generateExpression(data, binary->rightOperand);
  } else if (binary->rightOperand->expressionType == Expression::ExpressionType::VariableAccess)
  {
    VariableAccess* identifier = ((VariableAccess*)binary->rightOperand);

    std::string_view typeName;
    switch (((Pointer*)declarations[leftOperandName.first])->dataType->generalType)
    {
      case DataType::GeneralType::Struct:
        typeName = ((Struct*)((Pointer*)declarations[leftOperandName.first])->dataType)->identifier;

        for (std::pair<Declaration*, uint8_t> member: ((Struct*)memberOffsets[std::string(typeName)].first)->members)
        {
          if (member.first->identifier == identifier->identifier)
          {
            declarations[uid.get("_Member", binary)] = member.first->dataType;
          }
        }
        break;
      default:
        std::cout << "IR generation error: \"" << ((VariableAccess*)binary->leftOperand)->identifier << "\" is not a struct\n";
        throw IRGenError();
        break;
    }

    /*for (std::pair<const std::string, DataType*> type: declarations)
    {
      if (type.second->generalType == DataType::GeneralType::Struct)
      {
        std::string_view structName = ((Struct*)type.second)->identifier;
        std::vector<std::pair<Declaration *, uint8_t>>& members = ((Struct*)type.second)->members;
        std::cout << "struct\n";
      }

      if (type.second->generalType == DataType::GeneralType::Struct &&
        ((Struct*)type.second)->identifier == typeName &&
        !((Struct*)type.second)->members.empty())
      {
        for (std::pair<Declaration*, uint8_t> member: ((Struct*)type.second)->members)
        {
          if (member.first->identifier == identifier->identifier)
          {
            declarations[uid.get("_Member", binary)] = member.first->dataType;
            break;
          }
        }
        break;
      }
    }*/

    std::pair<PrimitiveType, uint8_t> memberData = memberOffsets[std::string(typeName)].second[std::string(identifier->identifier)];

    data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::Set, {uid.get("_Casted", binary), leftOperandName.first}});

    data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::SetAddition, {uid.get("_Pointer", binary), uid.get("_Casted", binary), std::to_string(memberData.second)}});

    data.instrArray->emplace_back(Operation{memberData.first, Operation::DereferenceRValue, {uid.get("_Member", binary), uid.get("_Pointer", binary)}});
  }

  if (binary->binaryType >= BinaryOperator::BinaryType::VariableAssignment &&
    binary->binaryType <= BinaryOperator::BinaryType::BitwiseXOREqual)
  {
    bool gotPointer = true;

    if (
      binary->leftOperand->expressionType == Expression::ExpressionType::PreUnaryOperator &&
      ((PreUnaryOperator*)binary->leftOperand)->preUnaryType == PreUnaryOperator::PreUnaryType::Dereference)
    {
      leftOperandName = generateExpression(data, ((PreUnaryOperator*)binary->leftOperand)->operand);

      DataType* resultType;
      if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Pointer)
      {
        resultType = ((Pointer*)declarations[leftOperandName.first])->dataType;
      } else if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Array)
      {
        resultType = ((Array*)declarations[leftOperandName.first])->dataType;
      }

      data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, resultType), Operation::DereferenceLValue, {leftOperandName.first, rightOperandName.first}});

    } else if (binary->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator)
    {
      if (((BinaryOperator*)binary->leftOperand)->binaryType == BinaryOperator::BinaryType::Subscript)
      {
        leftOperandName = generateExpression(data, ((BinaryOperator*)binary->leftOperand)->leftOperand);
        std::pair<std::string, PrimitiveType> childRightOperandName = generateExpression(data, ((BinaryOperator*)binary->leftOperand)->rightOperand);

        data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::SetAddition, {uid.get("_Added", binary), leftOperandName.first, childRightOperandName.first}});

        DataType* resultType;
        if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Pointer)
        {
          resultType = ((Pointer*)declarations[leftOperandName.first])->dataType;
        } else if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Array)
        {
          resultType = ((Array*)declarations[leftOperandName.first])->dataType;
        }

        data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, resultType), Operation::DereferenceLValue, {uid.get("_Added", binary), rightOperandName.first}});
      } else if (((BinaryOperator*)binary->leftOperand)->binaryType == BinaryOperator::BinaryType::DereferenceMemberAccess)
      {
        leftOperandName = generateExpression(data, ((BinaryOperator*)binary->leftOperand)->leftOperand);

        VariableAccess* identifier = ((VariableAccess*)((BinaryOperator*)binary->leftOperand)->rightOperand);

        std::string_view typeName;
        switch (((Pointer*)declarations[leftOperandName.first])->dataType->generalType)
        {
          case DataType::GeneralType::Struct:
            typeName = ((Struct*)((Pointer*)declarations[leftOperandName.first])->dataType)->identifier;
            break;
          default:
            std::cout << "IR generation error: \"" << ((VariableAccess*)binary->leftOperand)->identifier << "\" is not a struct\n";
            throw IRGenError();
            break;
        }

        /*for (std::pair<const std::string, DataType*> type: declarations)
        {
          if (type.second->generalType == DataType::GeneralType::Struct &&
            ((Struct*)type.second)->identifier == *typeName &&
            !((Struct*)type.second)->members.empty())
          {
            for (std::pair<Declaration*, uint8_t> member: ((Struct*)type.second)->members)
            {
              if (member.first->identifier == identifier->identifier)
              {
                declarations[uid.get("_Member", binary)] = member.first->dataType;
                break;
              }
            }
            break;
          }
        }*/

        std::pair<PrimitiveType, uint8_t> memberData = memberOffsets[std::string(typeName)].second[std::string(identifier->identifier)];

        data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::Set, {uid.get("_Casted", binary), leftOperandName.first}});

        data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::SetAddition, {uid.get("_Pointer", binary), uid.get("_Casted", binary), std::to_string(memberData.second)}});

        leftOperandName = {uid.get("_Pointer", binary), memberData.first};

        data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::DereferenceLValue, {leftOperandName.first, rightOperandName.first}});
      } else
      {
        gotPointer = false;
      }
    } else
    {
      gotPointer = false;
    }

    if (gotPointer)
    {
      declarations[uid.get("_Assigned", binary)] = declarations[rightOperandName.first];
      data.instrArray->emplace_back(Operation{rightOperandName.second, Operation::Set, {uid.get("_Assigned", binary), rightOperandName.first}});
      return {data.instrArray->back().operands[0], data.instrArray->back().type};
    }
  }

  switch (binary->binaryType)
  {
    case BinaryOperator::BinaryType::VariableAssignment:
      if (leftOperandName.first[0] < '0' || leftOperandName.first[0] > '9' || leftOperandName.first.find("_Member") != std::string::npos)
      {
        declarations[uid.get("_Assigned", binary)] = declarations[rightOperandName.first];

        data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::Set, {leftOperandName.first, rightOperandName.first}});
        data.instrArray->emplace_back(Operation{rightOperandName.second, Operation::Set, {uid.get("_Assigned", binary), rightOperandName.first}});
      } else
      {
        std::cout << "IR generation error: Assigning rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case BinaryOperator::BinaryType::AddEqual:
    case BinaryOperator::BinaryType::SubtractEqual:
    case BinaryOperator::BinaryType::MultiplyEqual:
    case BinaryOperator::BinaryType::DivideEqual:
    case BinaryOperator::BinaryType::ModuloEqual:
    case BinaryOperator::BinaryType::LeftShiftEqual:
    case BinaryOperator::BinaryType::RightShiftEqual:
    case BinaryOperator::BinaryType::BitwiseOREqual:
    case BinaryOperator::BinaryType::BitwiseANDEqual:
    case BinaryOperator::BinaryType::BitwiseXOREqual:
      if (leftOperandName.first[0] < '0' || leftOperandName.first[0] > '9' || leftOperandName.first.find("_Member") != std::string::npos)
      {
        Operation::Opcode operationType = Operation::Opcode((int)Operation::SetAddition + ((int)binary->binaryType - (int)BinaryOperator::BinaryType::AddEqual));

        declarations[uid.get("_Assigned", binary)] = declarations[rightOperandName.first];

        data.instrArray->emplace_back(Operation{leftOperandName.second, operationType, {leftOperandName.first, leftOperandName.first, rightOperandName.first}});
        data.instrArray->emplace_back(Operation{rightOperandName.second, Operation::Set, {uid.get("_Assigned", binary), leftOperandName.first}});
      } else
      {
        std::cout << "IR generation error: Modifying rvalue is illegal\n";
        throw IRGenError();
      }
      break;
    case BinaryOperator::BinaryType::Add:
      declarations[uid.get("_Added", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetAddition, {uid.get("_Added", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Subtract:
      declarations[uid.get("_Subtracted", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetSubtraction, {uid.get("_Subtracted", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Multiply:
      declarations[uid.get("_Multiplied", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetMultiplication, {uid.get("_Multiplied", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Divide:
      declarations[uid.get("_Divided", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetDivision, {uid.get("_Divided", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Modulo:
      declarations[uid.get("_Modulus", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetModulo, {uid.get("_Modulus", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LeftShift:
      declarations[uid.get("_LeftShifted", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetLeftShift, {uid.get("_LeftShifted", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::RightShift:
      declarations[uid.get("_RightShifted", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetRightShift, {uid.get("_RightShifted", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseOR:
      declarations[uid.get("_BitwiseOred", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseOR, {uid.get("_BitwiseOred", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseAND:
      declarations[uid.get("_BitwiseAnded", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseAND, {uid.get("_BitwiseAnded", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::BitwiseXOR:
      declarations[uid.get("_BitwiseXored", binary)] = declarations[leftOperandName.first];

      data.instrArray->emplace_back(Operation{leftOperandName.second, Operation::SetBitwiseXOR, {uid.get("_BitwiseXored", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LogicalOR:
      declarations[uid.get("_LogicalOred", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_LogicalOred", binary)]);
      ((PrimitiveType*)declarations[uid.get("_LogicalOred", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_LogicalOred", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetLogicalOR, {uid.get("_LogicalOred", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LogicalAND:
      declarations[uid.get("_LogicalAnded", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_LogicalAnded", binary)]);
      ((PrimitiveType*)declarations[uid.get("_LogicalAnded", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_LogicalAnded", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetLogicalAND, {uid.get("_LogicalAnded", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Subscript:
      DataType* resultType;
      if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Pointer)
      {
        resultType = ((Pointer*)declarations[leftOperandName.first])->dataType;
      } else if (declarations[leftOperandName.first]->generalType == DataType::GeneralType::Array)
      {
        resultType = ((Array*)declarations[leftOperandName.first])->dataType;
      }
      declarations[uid.get("_Dereference", binary)] = resultType;

      data.instrArray->emplace_back(Operation{{data.typeSizes.pointerSize, data.typeSizes.pointerSize}, Operation::SetAddition, {uid.get("_Added", binary), leftOperandName.first, rightOperandName.first}});

      data.instrArray->emplace_back(Operation{ASTTypeToIRType(data, resultType), Operation::DereferenceRValue, {uid.get("_Dereference", binary), data.instrArray->back().operands[0]}});
      break;
    case BinaryOperator::BinaryType::Equal:
      declarations[uid.get("_Equaled", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_Equaled", binary)]);
      ((PrimitiveType*)declarations[uid.get("_Equaled", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_Equaled", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetEqual, {uid.get("_Equaled", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::NotEqual:
      declarations[uid.get("_NotEqualed", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_NotEqualed", binary)]);
      ((PrimitiveType*)declarations[uid.get("_NotEqualed", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_NotEqualed", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetNotEqual, {uid.get("_NotEqualed", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Greater:
      declarations[uid.get("_Large", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_Large", binary)]);
      ((PrimitiveType*)declarations[uid.get("_Large", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_Large", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetGreater, {uid.get("_Large", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::Lesser:
      declarations[uid.get("_Small", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_Small", binary)]);
      ((PrimitiveType*)declarations[uid.get("_Small", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_Small", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetLesser, {uid.get("_Small", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::GreaterOrEqual:
      declarations[uid.get("_Largeish", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_Largeish", binary)]);
      ((PrimitiveType*)declarations[uid.get("_Largeish", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_Largeish", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetGreaterOrEqual, {uid.get("_Largeish", binary), leftOperandName.first, rightOperandName.first}});
      break;
    case BinaryOperator::BinaryType::LesserOrEqual:
      declarations[uid.get("_Smallish", binary)] = arenaAlloc((PrimitiveType*)declarations[uid.get("_Smallish", binary)]);
      ((PrimitiveType*)declarations[uid.get("_Smallish", binary)])->size = 1;
      ((PrimitiveType*)declarations[uid.get("_Smallish", binary)])->alignment = 1;

      data.instrArray->emplace_back(Operation{{1, 1}, Operation::SetLesserOrEqual, {uid.get("_Smallish", binary), leftOperandName.first, rightOperandName.first}});
      break;
  }

  return {data.instrArray->back().operands[0], data.instrArray->back().type};
}

std::pair<std::string, PrimitiveType> GenerateIR::generateTernaryOperator(CommonIRData& data, const TernaryOperator* ternary)
{
  // conditional jump to end of if conditional
  std::pair<std::string, PrimitiveType> name = generateExpression(data, ternary->condition);

  data.instrArray->emplace_back(Operation{name.second, Operation::JumpIfZero, {uid.get("_TernaryOperatorFalse", ternary), name.first}});

  // if conditional body
  std::pair<std::string, PrimitiveType> trueName = generateExpression(data, ternary->trueOperand);
  data.instrArray->emplace_back(Operation{trueName.second, Operation::Set, {name.first, trueName.first}});

  // unconditional jump to end of else conditional
  data.instrArray->emplace_back(Operation{{}, Operation::Jump, {uid.get("_TernaryOperatorEnd", ternary)}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_TernaryOperatorFalse", ternary)}});

  // else body
  std::pair<std::string, PrimitiveType> falseName = generateExpression(data, ternary->falseOperand);
  data.instrArray->emplace_back(Operation{falseName.second, Operation::Set, {name.first, falseName.first}});

  data.instrArray->emplace_back(Operation{{}, Operation::Label, {uid.get("_TernaryOperatorEnd", ternary)}});

  return name;
}

bool GenerateIR::resolveConstantOperations(IRprogram& irProgram)
{
  bool changed = false;

  std::map<std::string, std::string> vars;

  for (std::vector<IRprogram::Function>::iterator f = irProgram.program.begin(); f != irProgram.program.end(); f++)
  {
    for (std::vector<Operation>::iterator i = f->body.begin(); i != f->body.end(); i++)
    {
      if (vars.contains(i->operands[1]))
      {
        i->operands[1] = vars[i->operands[1]];
        changed = true;
      } else if (vars.contains(i->operands[2]))
      {
        i->operands[2] = vars[i->operands[2]];
        changed = true;
      }

      switch (i->code)
      {
        case Operation::Set:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = i->operands[1];
          }
          break;
        case Operation::GetAddress:

          break;
        case Operation::Return:
          if (vars.contains(i->operands[0]))
          {
            i->operands[0] = vars[i->operands[0]];
            changed = true;
          }
          break;
        case Operation::AddArg:
          if (vars.contains(i->operands[0]))
          {
            i->operands[0] = vars[i->operands[0]];
            changed = true;
          }
          break;
        case Operation::JumpIfZero:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            if (std::stoi(i->operands[1]) == 0)
            {
              i->code = Operation::Jump;
            } else
            {
              f->body.erase(i--);
            }
            changed = true;
          }
          break;
        case Operation::JumpIfNotZero:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            if (std::stoi(i->operands[1]) != 0)
            {
              i->code = Operation::Jump;
            } else
            {
              f->body.erase(i--);
            }
            changed = true;
          }
          break;
        case Operation::DereferenceLValue:

          break;
        case Operation::DereferenceRValue:

          break;
        case Operation::SetAddition:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) + std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetSubtraction:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) - std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetMultiplication:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) * std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetDivision:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) / std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetModulo:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) % std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetBitwiseAND:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) & std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetBitwiseOR:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) | std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetBitwiseXOR:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) ^ std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetLeftShift:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) << std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetRightShift:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) >> std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetLogicalAND:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) && std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetLogicalOR:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) || std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetEqual:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) == std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetNotEqual:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) != std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetGreater:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) > std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetLesser:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) < std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetGreaterOrEqual:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) >= std::stoi(i->operands[2]));
          }
          break;
        case Operation::SetLesserOrEqual:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos && i->operands[2].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) <= std::stoi(i->operands[2]));
          }
          break;
        case Operation::Negate:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(-std::stoi(i->operands[1]));
          }
          break;
        case Operation::LogicalNOT:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(!std::stoi(i->operands[1]));
          }
          break;
        case Operation::BitwiseNOT:
          if (i->operands[1].find_first_not_of(".0123456789") == std::string::npos)
          {
            vars[i->operands[0]] = std::to_string(~std::stoi(i->operands[1]));
          }
          break;
        case Operation::Label:
          vars.clear();
          break;
      }
    }
  }

  return changed;
}

bool GenerateIR::trimInaccessibleCode(IRprogram& irProgram)
{
  bool changed = false;

  struct IdentifierUsage
  {
    bool declared: 1;
    bool used: 1;

    IdentifierUsage()
    {
      declared = false;
      used = false;
    }
  };
  std::map<std::string, IdentifierUsage> identifiers;

  for (std::vector<IRprogram::Function>::iterator f = irProgram.program.begin(); f != irProgram.program.end(); f++)
  {
    bool accessible = true;

    uint16_t index = 0;
    for (std::vector<Operation>::iterator i = f->body.begin(); i != f->body.end(); i++)
    {
      if (i->code == Operation::Label)
      {
        //identifiers[i->operands[0]].declared = true;
        accessible = true;
      }

      if (!accessible)
      {
        f->body.erase(i);
        changed = true;
        break;
      }

      if (i->code == Operation::Jump || i->code == Operation::Return)
      {
        accessible = false;
      }

      if (i->code == Operation::Jump || i->code == Operation::JumpIfZero || i->code == Operation::JumpIfNotZero || i->code == Operation::Call)
      {
        identifiers[i->operands[0]].used = true;
      }

      if (i->code == Operation::Set || 
        i->code == Operation::DereferenceLValue || 
        i->code == Operation::DereferenceRValue || 
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
        identifiers[i->operands[0]].declared = true;

        if (i->type.isVolatile)
        {
          identifiers[i->operands[0]].used = true;
        }
        if (!i->operands[1].empty() && i->operands[1].find_first_not_of(".0123456789") != std::string::npos)
        {
          identifiers[i->operands[1]].used = true;
        }
        if (!i->operands[2].empty() && i->operands[2].find_first_not_of(".0123456789") != std::string::npos)
        {
          identifiers[i->operands[2]].used = true;
        }
      } else if (i->code != Operation::Label && 
        i->code != Operation::Jump)
      {
        if (!i->operands[0].empty() && i->operands[0].find_first_not_of(".0123456789") != std::string::npos)
        {
          identifiers[i->operands[0]].used = true;
        }
        if (!i->operands[1].empty() && i->operands[1].find_first_not_of(".0123456789") != std::string::npos)
        {
          identifiers[i->operands[1]].used = true;
        }
        if (!i->operands[2].empty() && i->operands[2].find_first_not_of(".0123456789") != std::string::npos)
        {
          identifiers[i->operands[2]].used = true;
        }
      }
      index++;
    }

    if (f->body.empty())
    {
      irProgram.program.erase(f);
      break;
    }
  }

  // This marks certain values to be kept
  while (!identifiers.empty() && (identifiers.begin()->second.declared == false || identifiers.begin()->second.used == true || identifiers.begin()->first == "main"))
  {
    identifiers.erase(identifiers.begin());
  }

  for (std::map<std::string, IdentifierUsage>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
  {
    if (l->second.declared == false || l->second.used == true || l->first == "main")
    {
      identifiers.erase(l--);
    }
  }

  if (identifiers.empty())
  {
    return changed;
  }

  for (std::vector<IRprogram::Function>::iterator f = irProgram.program.begin(); f != irProgram.program.end(); f++)
  {
    for (std::vector<Operation>::iterator i = f->body.begin(); i != f->body.end(); i++)
    {
      if (i->code == Operation::Set || 
        i->code == Operation::DereferenceLValue || 
        i->code == Operation::DereferenceRValue || 
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
        for (std::map<std::string, IdentifierUsage>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
        {
          if (i->operands[0] == l->first)
          {
            f->body.erase(i--);
            changed = true;
          }
        }
      }
    }
  }

  return changed;
}
